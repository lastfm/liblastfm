/*
   Copyright 2009-2010 Last.fm Ltd.
      - Primarily authored by Max Howell, Jono Cole, Doug Mansell and Michael Coffey

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
#pragma once

#include <QUrl>
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
#include <QUrlQuery>
#endif

#include "global.h"

namespace lastfm
{
    class LASTFM_DLLEXPORT Url
    {
    public:
        explicit Url( const QUrl& url );

        void addQueryItem( const QString& key, const QString& value );

        QUrl url() const;

        QUrl operator()();

    private:
        QUrl m_url;
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
        QUrlQuery m_query;
#endif
    };
}

