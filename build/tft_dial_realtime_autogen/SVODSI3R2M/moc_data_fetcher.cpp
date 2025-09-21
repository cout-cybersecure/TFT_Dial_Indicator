/****************************************************************************
** Meta object code from reading C++ file 'data_fetcher.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../cpp/data_fetcher.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'data_fetcher.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_DataFetcher_t {
    uint offsetsAndSizes[28];
    char stringdata0[12];
    char stringdata1[5];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[6];
    char stringdata5[11];
    char stringdata6[16];
    char stringdata7[11];
    char stringdata8[2];
    char stringdata9[13];
    char stringdata10[12];
    char stringdata11[29];
    char stringdata12[26];
    char stringdata13[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DataFetcher_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DataFetcher_t qt_meta_stringdata_DataFetcher = {
    {
        QT_MOC_LITERAL(0, 11),  // "DataFetcher"
        QT_MOC_LITERAL(12, 4),  // "tick"
        QT_MOC_LITERAL(17, 0),  // ""
        QT_MOC_LITERAL(18, 5),  // "ts_ms"
        QT_MOC_LITERAL(24, 5),  // "price"
        QT_MOC_LITERAL(30, 10),  // "do_connect"
        QT_MOC_LITERAL(41, 15),  // "on_ws_connected"
        QT_MOC_LITERAL(57, 10),  // "on_ws_text"
        QT_MOC_LITERAL(68, 1),  // "m"
        QT_MOC_LITERAL(70, 12),  // "on_ws_closed"
        QT_MOC_LITERAL(83, 11),  // "on_ws_error"
        QT_MOC_LITERAL(95, 28),  // "QAbstractSocket::SocketError"
        QT_MOC_LITERAL(124, 25),  // "on_backfill_http_finished"
        QT_MOC_LITERAL(150, 12)   // "on_reconnect"
    },
    "DataFetcher",
    "tick",
    "",
    "ts_ms",
    "price",
    "do_connect",
    "on_ws_connected",
    "on_ws_text",
    "m",
    "on_ws_closed",
    "on_ws_error",
    "QAbstractSocket::SocketError",
    "on_backfill_http_finished",
    "on_reconnect"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DataFetcher[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   62,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    0,   67,    2, 0x08,    4 /* Private */,
       6,    0,   68,    2, 0x08,    5 /* Private */,
       7,    1,   69,    2, 0x08,    6 /* Private */,
       9,    0,   72,    2, 0x08,    8 /* Private */,
      10,    1,   73,    2, 0x08,    9 /* Private */,
      12,    0,   76,    2, 0x08,   11 /* Private */,
      13,    0,   77,    2, 0x08,   12 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::LongLong, QMetaType::Double,    3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject DataFetcher::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DataFetcher.offsetsAndSizes,
    qt_meta_data_DataFetcher,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DataFetcher_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DataFetcher, std::true_type>,
        // method 'tick'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'do_connect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_ws_connected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_ws_text'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_ws_closed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_ws_error'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketError, std::false_type>,
        // method 'on_backfill_http_finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_reconnect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void DataFetcher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DataFetcher *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->tick((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 1: _t->do_connect(); break;
        case 2: _t->on_ws_connected(); break;
        case 3: _t->on_ws_text((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->on_ws_closed(); break;
        case 5: _t->on_ws_error((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 6: _t->on_backfill_http_finished(); break;
        case 7: _t->on_reconnect(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DataFetcher::*)(qint64 , double );
            if (_t _q_method = &DataFetcher::tick; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *DataFetcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataFetcher::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DataFetcher.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DataFetcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void DataFetcher::tick(qint64 _t1, double _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
