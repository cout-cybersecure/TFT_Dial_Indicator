/****************************************************************************
** Meta object code from reading C++ file 'TrainerInvoker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../cpp/TrainerInvoker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TrainerInvoker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TrainerInvoker_t {
    QByteArrayData data[15];
    char stringdata0[123];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TrainerInvoker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TrainerInvoker_t qt_meta_stringdata_TrainerInvoker = {
    {
QT_MOC_LITERAL(0, 0, 14), // "TrainerInvoker"
QT_MOC_LITERAL(1, 15, 6), // "status"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 4), // "line"
QT_MOC_LITERAL(4, 28, 10), // "finishedOk"
QT_MOC_LITERAL(5, 39, 8), // "onnxPath"
QT_MOC_LITERAL(6, 48, 6), // "failed"
QT_MOC_LITERAL(7, 55, 6), // "reason"
QT_MOC_LITERAL(8, 62, 7), // "onReady"
QT_MOC_LITERAL(9, 70, 10), // "onFinished"
QT_MOC_LITERAL(10, 81, 4), // "code"
QT_MOC_LITERAL(11, 86, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(12, 107, 2), // "st"
QT_MOC_LITERAL(13, 110, 5), // "start"
QT_MOC_LITERAL(14, 116, 6) // "symbol"

    },
    "TrainerInvoker\0status\0\0line\0finishedOk\0"
    "onnxPath\0failed\0reason\0onReady\0"
    "onFinished\0code\0QProcess::ExitStatus\0"
    "st\0start\0symbol"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TrainerInvoker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    1,   47,    2, 0x06 /* Public */,
       6,    1,   50,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,   53,    2, 0x08 /* Private */,
       9,    2,   54,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      13,    1,   59,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 11,   10,   12,

 // methods: parameters
    QMetaType::Void, QMetaType::QString,   14,

       0        // eod
};

void TrainerInvoker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TrainerInvoker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->status((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->finishedOk((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->failed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->onReady(); break;
        case 4: _t->onFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 5: _t->start((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TrainerInvoker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TrainerInvoker::status)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TrainerInvoker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TrainerInvoker::finishedOk)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TrainerInvoker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TrainerInvoker::failed)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TrainerInvoker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_TrainerInvoker.data,
    qt_meta_data_TrainerInvoker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TrainerInvoker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TrainerInvoker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TrainerInvoker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TrainerInvoker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void TrainerInvoker::status(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TrainerInvoker::finishedOk(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TrainerInvoker::failed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
