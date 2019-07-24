/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: moc_FeatureExtractorThread.cpp

Author: Anand Seethepalli (aseethepalli@noble.org)
Principal Investigator: Larry York (lmyork@noble.org)
Root Phenomics Lab
Noble Research Institute, LLC

This file is part of RhizoVision Analyzer.

RhizoVision Analyzer is free software: you can redistribute it and/or modify
it under the terms of the NOBLE RESEARCH INSTITUTE, GENERAL PUBLIC LICENSE.

RhizoVision Analyzer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
NOBLE RESEARCH INSTITUTE GENERAL PUBLIC LICENSE for more details.

You should have received a copy of the Noble Research Institute General Public License
along with RhizoVision Analyzer.  If not, see <https://github.com/noble-research-institute/RhizoVisionAnalyzer/blob/master/LICENSE>.
*/

/****************************************************************************
** Meta object code from reading C++ file 'FeatureExtractorThread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "FeatureExtractorThread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FeatureExtractorThread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FeatureExtractorThread_t {
    QByteArrayData data[5];
    char stringdata0[55];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FeatureExtractorThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FeatureExtractorThread_t qt_meta_stringdata_FeatureExtractorThread = {
    {
QT_MOC_LITERAL(0, 0, 22), // "FeatureExtractorThread"
QT_MOC_LITERAL(1, 23, 14), // "ReportProgress"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 8), // "filename"
QT_MOC_LITERAL(4, 48, 6) // "fileno"

    },
    "FeatureExtractorThread\0ReportProgress\0"
    "\0filename\0fileno"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FeatureExtractorThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    3,    4,

       0        // eod
};

void FeatureExtractorThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FeatureExtractorThread *_t = static_cast<FeatureExtractorThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ReportProgress((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FeatureExtractorThread::*_t)(QString , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FeatureExtractorThread::ReportProgress)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject FeatureExtractorThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_FeatureExtractorThread.data,
      qt_meta_data_FeatureExtractorThread,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *FeatureExtractorThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FeatureExtractorThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FeatureExtractorThread.stringdata0))
        return static_cast<void*>(const_cast< FeatureExtractorThread*>(this));
    return QThread::qt_metacast(_clname);
}

int FeatureExtractorThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void FeatureExtractorThread::ReportProgress(QString _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
