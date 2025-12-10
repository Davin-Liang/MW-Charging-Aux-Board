/****************************************************************************
** Meta object code from reading C++ file 'tab_turntablecontrol.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "tab_turntablecontrol.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tab_turntablecontrol.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TabTurntableControl_t {
    QByteArrayData data[17];
    char stringdata0[412];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TabTurntableControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TabTurntableControl_t qt_meta_stringdata_TabTurntableControl = {
    {
QT_MOC_LITERAL(0, 0, 19), // "TabTurntableControl"
QT_MOC_LITERAL(1, 20, 22), // "update_turntable_image"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 24), // "on_btn_set_speed_clicked"
QT_MOC_LITERAL(4, 69, 35), // "on_btn_stop_x_turntable_run_c..."
QT_MOC_LITERAL(5, 105, 35), // "on_btn_stop_y_turntable_run_c..."
QT_MOC_LITERAL(6, 141, 24), // "on_btn_set_x_pos_clicked"
QT_MOC_LITERAL(7, 166, 24), // "on_btn_set_y_pos_clicked"
QT_MOC_LITERAL(8, 191, 21), // "on_btn_x_zero_clicked"
QT_MOC_LITERAL(9, 213, 21), // "on_btn_y_zero_clicked"
QT_MOC_LITERAL(10, 235, 29), // "on_btn_set_target_pos_clicked"
QT_MOC_LITERAL(11, 265, 42), // "on_btn_set_pidcontroller_para..."
QT_MOC_LITERAL(12, 308, 19), // "updateTurntableData"
QT_MOC_LITERAL(13, 328, 14), // "closedLoopTick"
QT_MOC_LITERAL(14, 343, 31), // "on_controller_selection_changed"
QT_MOC_LITERAL(15, 375, 5), // "index"
QT_MOC_LITERAL(16, 381, 30) // "on_btn_stop_pidcontrol_clicked"

    },
    "TabTurntableControl\0update_turntable_image\0"
    "\0on_btn_set_speed_clicked\0"
    "on_btn_stop_x_turntable_run_clicked\0"
    "on_btn_stop_y_turntable_run_clicked\0"
    "on_btn_set_x_pos_clicked\0"
    "on_btn_set_y_pos_clicked\0on_btn_x_zero_clicked\0"
    "on_btn_y_zero_clicked\0"
    "on_btn_set_target_pos_clicked\0"
    "on_btn_set_pidcontroller_parameter_clicked\0"
    "updateTurntableData\0closedLoopTick\0"
    "on_controller_selection_changed\0index\0"
    "on_btn_stop_pidcontrol_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TabTurntableControl[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x0a /* Public */,
       3,    0,   85,    2, 0x0a /* Public */,
       4,    0,   86,    2, 0x0a /* Public */,
       5,    0,   87,    2, 0x0a /* Public */,
       6,    0,   88,    2, 0x0a /* Public */,
       7,    0,   89,    2, 0x0a /* Public */,
       8,    0,   90,    2, 0x0a /* Public */,
       9,    0,   91,    2, 0x0a /* Public */,
      10,    0,   92,    2, 0x0a /* Public */,
      11,    0,   93,    2, 0x0a /* Public */,
      12,    0,   94,    2, 0x0a /* Public */,
      13,    0,   95,    2, 0x0a /* Public */,
      14,    1,   96,    2, 0x0a /* Public */,
      16,    0,   99,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void,

       0        // eod
};

void TabTurntableControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TabTurntableControl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->update_turntable_image(); break;
        case 1: _t->on_btn_set_speed_clicked(); break;
        case 2: _t->on_btn_stop_x_turntable_run_clicked(); break;
        case 3: _t->on_btn_stop_y_turntable_run_clicked(); break;
        case 4: _t->on_btn_set_x_pos_clicked(); break;
        case 5: _t->on_btn_set_y_pos_clicked(); break;
        case 6: _t->on_btn_x_zero_clicked(); break;
        case 7: _t->on_btn_y_zero_clicked(); break;
        case 8: _t->on_btn_set_target_pos_clicked(); break;
        case 9: _t->on_btn_set_pidcontroller_parameter_clicked(); break;
        case 10: _t->updateTurntableData(); break;
        case 11: _t->closedLoopTick(); break;
        case 12: _t->on_controller_selection_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->on_btn_stop_pidcontrol_clicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TabTurntableControl::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_TabTurntableControl.data,
    qt_meta_data_TabTurntableControl,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TabTurntableControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TabTurntableControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TabTurntableControl.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TabTurntableControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
