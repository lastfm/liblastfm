/*
   Copyright 2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of liblastfm.

   liblastfm is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   liblastfm is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with liblastfm.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "RadioTuner.h"
#include "RadioStation.h"
#include "XmlQuery.h"
#include "Xspf.h"
#include "ws.h"

#include <QDebug>
#include <QTimer>

using namespace lastfm;

//TODO skips left
//TODO multiple locations for the same track
//TODO set rtp flag in getPlaylist (whether user is scrobbling this radio session or not)

// limit the number of retries following empty playlists:
#define MAX_TUNING_ATTEMPTS 3


class lastfm::RadioTunerPrivate : public QObject
{
    Q_OBJECT
    public:
        QList<Xspf*> m_playlistQueue;
        uint m_retry_counter;
        bool m_fetchingPlaylist;
        bool m_requestedPlaylist;
        class QTimer* m_twoSecondTimer;
        RadioStation m_station;
        RadioStation m_retuneStation;

        RadioTunerPrivate( QObject * parent, const RadioStation& station );

        /** Tries again up to 5 times 
          * @returns true if we tried again, otherwise you should emit error */
        bool tryAgain();
        /** Will emit 5 tracks from tracks(), they have to played within an hour
          * or the streamer will refuse to stream them. Also the previous five are
          * invalidated apart from the one that is currently playing, so sorry, you
          * can't build up big lists of tracks.
          *
          * I feel I must point out that asking the user which one they want to play
          * is also not allowed according to our terms and conditions, which you
          * already agreed to in order to get your API key. Sorry about that dude. 
          */
        void fetchFiveMoreTracks();

    private slots:
        void onTwoSecondTimeout();
};


lastfm::RadioTunerPrivate::RadioTunerPrivate( QObject *parent, const RadioStation& station )
    : QObject( parent ),
    m_station( station )
{
    m_retry_counter = 0;
    m_fetchingPlaylist = false;
    m_requestedPlaylist = false;
    m_twoSecondTimer = new QTimer( this );
    m_twoSecondTimer->setSingleShot( true );
    connect( m_twoSecondTimer, SIGNAL(timeout()), SLOT(onTwoSecondTimeout()));
}


void
RadioTunerPrivate::onTwoSecondTimeout()
{
    if (m_requestedPlaylist)
    {
        m_requestedPlaylist = false;
        fetchFiveMoreTracks();
    }
}



void
RadioTunerPrivate::fetchFiveMoreTracks()
{
    if ( !m_retuneStation.url().isEmpty() )
    {
        // We have been asked to retune so do it now
        QMap<QString, QString> map;
        map["method"] = "radio.tune";
        map["station"] = m_retuneStation.url();
        map["additional_info"] = "1";

        QNetworkReply* reply = ws::post(map);
        connect( reply, SIGNAL(finished()), parent(), SLOT(onTuneReturn()) );

        m_retuneStation = RadioStation();
        m_twoSecondTimer->stop();
    }
    else
    {
        if ( !m_twoSecondTimer->isActive() )
        {
            //TODO check documentation, I figure this needs a session key
            QMap<QString, QString> map;
            map["method"] = "radio.getPlaylist";
            map["additional_info"] = "1";
            map["rtp"] = "1"; // see above
            connect( ws::post( map ), SIGNAL(finished()), parent(), SLOT(onGetPlaylistReturn()) );
            m_fetchingPlaylist = true;
        }
        else
            m_requestedPlaylist = true;
    }
}


bool
RadioTunerPrivate::tryAgain()
{
    qDebug() << "Bad response count" << m_retry_counter;
    
    if (++m_retry_counter > MAX_TUNING_ATTEMPTS)
        return false;
    fetchFiveMoreTracks();
    return true;
}


RadioTuner::RadioTuner( const RadioStation& station )
    :d( new RadioTunerPrivate( this, station ) )
{
    qDebug() << station.url();

    //Empty RadioStation implies that the radio
    //should tune to the previous station.
    if( station.url().isEmpty() )
    {
        d->fetchFiveMoreTracks();
    }
    else
    {
        QMap<QString, QString> map;
        map["method"] = "radio.tune";
        map["station"] = station.url();
        map["additional_info"] = "1";
        connect( ws::post(map), SIGNAL(finished()), SLOT(onTuneReturn()) );
    }
}

RadioTuner::~RadioTuner()
{
}

void
RadioTuner::retune( const RadioStation& station )
{
    d->m_playlistQueue.clear();
    d->m_retuneStation = station;

    qDebug() << station.url();
}


void
RadioTuner::onTuneReturn()
{
    if ( !d->m_retuneStation.url().isEmpty() )
        d->m_station = d->m_retuneStation;

    XmlQuery lfm;

    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender()) ) )
    {
        qDebug() << lfm;

        d->m_station.setTitle( lfm["station"]["name"].text() );
        d->m_station.setUrl( lfm["station"]["url"].text() );

        emit title( lfm["station"]["name"].text() );
        emit supportsDisco( lfm["station"]["supportsdiscovery"].text() == "1" );
        d->fetchFiveMoreTracks();
    }
    else
    {
        emit error( lfm.parseError().enumValue(), lfm.parseError().message() );
    }
}


void
RadioTuner::onGetPlaylistReturn()
{   
    // We shouldn't request another playlist for 2 seconds because we'll get the same one
    // in a different order. This QTimer will block until it has finished. If one or more
    // playlists have been requested in the meantime, it will fetch one on timeout
    d->m_twoSecondTimer->start( 2000 );

    // This will block us fetching two playlists at once
    d->m_fetchingPlaylist = false;

    XmlQuery lfm;

    if ( lfm.parse( qobject_cast<QNetworkReply*>(sender()) ) )
    {
        qDebug() << lfm;

        d->m_station.setTitle( lfm["playlist"]["title"].text() );
        // we don't get the radio url in the playlist
        //d->m_station.setUrl( lfm["station"]["url"].text() );

        emit title( lfm["playlist"]["title"].text() );

        Xspf* xspf = new Xspf( lfm["playlist"], this );
        connect( xspf, SIGNAL(expired()), SLOT(onXspfExpired()) );

        if ( xspf->isEmpty() )
        {
            // give up after too many empty playlists  :(
            if (!d->tryAgain())
                emit error( ws::NotEnoughContent, tr("Not enough content") );
        }
        else
        {
            d->m_retry_counter = 0;
            d->m_playlistQueue << xspf;
            emit trackAvailable();
        }
    }
    else
    {
        qDebug() << lfm.parseError().message() << lfm.parseError().enumValue();
        emit error( lfm.parseError().enumValue(), lfm.parseError().message() );
    }
}

void
RadioTuner::onXspfExpired()
{
    int index = d->m_playlistQueue.indexOf( static_cast<Xspf*>(sender()) );
    if ( index != -1 )
        d->m_playlistQueue.takeAt( index )->deleteLater();
}

Track
RadioTuner::takeNextTrack()
{
    if ( d->m_playlistQueue.isEmpty() )
    {
        // If there are no tracks here and we're not fetching tracks
        // it's probably because the playlist expired so fetch more now
        if ( !d->m_fetchingPlaylist )
            d->fetchFiveMoreTracks();

        return Track();
    }

    Track result = d->m_playlistQueue[0]->takeFirst();

    if ( d->m_playlistQueue[0]->isEmpty() )
        d->m_playlistQueue.removeFirst();

    if ( d->m_playlistQueue.isEmpty() )
        d->fetchFiveMoreTracks();

    return result;
}

#include "RadioTuner.moc"
