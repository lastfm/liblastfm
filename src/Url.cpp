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


lastfm::Url::Url( const QUrl& url )
    :m_url( url )
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
    , m_query( url.query() )
#endif
{

}

void
lastfm::Url::addQueryItem( const QByteArray& key, const QByteArray& value )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
    m_query.addQueryItem( key, value );
    m_url.setQuery( m_query );
#else
    m_url.addEncodedQueryItem( key, value );
#endif
}

QUrl
lastfm::Url::operator()()
{
    return url();
}

QUrl
lastfm::Url::url() const
{
    return m_url;
}