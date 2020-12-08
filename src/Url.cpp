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

#include "Url.h"

#include <QUrlQuery>

class lastfm::UrlPrivate
{
    public:
        UrlPrivate( const QUrl& url );
        QUrl url;
        QUrlQuery query;
};

lastfm::UrlPrivate::UrlPrivate( const QUrl& u )
    : url( u )
    , query( u.query() )
{
}

lastfm::Url::Url( const QUrl& url )
    :d( new UrlPrivate( url ) )
{
}

lastfm::Url::~Url()
{
    delete d;
}

void
lastfm::Url::addQueryItem( const QString& key, const QString& value )
{
    d->query.addQueryItem( key, value );
    d->url.setQuery( d->query );
}

QUrl
lastfm::Url::operator()()
{
    return url();
}

lastfm::Url&
lastfm::Url::operator=( const lastfm::Url& that )
{
    d->url = that.d->url;
    d->query = that.d->query;
    return *this;
}


QUrl
lastfm::Url::url() const
{
    return d->url;
}
