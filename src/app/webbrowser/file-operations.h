/*
 * Copyright 2014-2015 Canonical Ltd.
 *
 * This file is part of webbrowser-app.
 *
 * webbrowser-app is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * webbrowser-app is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __FILE_OPERATIONS_H__
#define __FILE_OPERATIONS_H__

#include <QtCore/QObject>

class QUrl;

class FileOperations : public QObject
{
    Q_OBJECT

public:
    explicit FileOperations(QObject* parent=0);

    Q_INVOKABLE bool exists(const QUrl& path) const;
    Q_INVOKABLE bool remove(const QUrl& file) const;
    Q_INVOKABLE bool mkpath(const QUrl& path) const;
};

#endif // __FILE_OPERATIONS_H__
