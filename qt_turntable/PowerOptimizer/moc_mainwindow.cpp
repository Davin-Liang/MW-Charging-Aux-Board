/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[37];
    char stringdata0[881];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 29), // "on_pushButton_connect_clicked"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 32), // "on_pushButton_disconnect_clicked"
QT_MOC_LITERAL(4, 75, 13), // "onSocketError"
QT_MOC_LITERAL(5, 89, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(6, 118, 5), // "error"
QT_MOC_LITERAL(7, 124, 35), // "on_pushButton_motor_control_c..."
QT_MOC_LITERAL(8, 160, 34), // "on_pushButton_find_optimal_cl..."
QT_MOC_LITERAL(9, 195, 22), // "initializeUIWithConfig"
QT_MOC_LITERAL(10, 218, 15), // "trajTypeChanged"
QT_MOC_LITERAL(11, 234, 5), // "index"
QT_MOC_LITERAL(12, 240, 19), // "onMotorDataReceived"
QT_MOC_LITERAL(13, 260, 11), // "MotorData_t"
QT_MOC_LITERAL(14, 272, 4), // "data"
QT_MOC_LITERAL(15, 277, 21), // "onChannelDataReceived"
QT_MOC_LITERAL(16, 299, 13), // "CurrentVPCh_t"
QT_MOC_LITERAL(17, 313, 20), // "onOptResDataReceived"
QT_MOC_LITERAL(18, 334, 12), // "OptResData_t"
QT_MOC_LITERAL(19, 347, 37), // "on_pushButton_filenamedisplay..."
QT_MOC_LITERAL(20, 385, 31), // "on_pushButton_read_file_clicked"
QT_MOC_LITERAL(21, 417, 40), // "on_listWidget_files_itemSelec..."
QT_MOC_LITERAL(22, 458, 22), // "update_turntable_image"
QT_MOC_LITERAL(23, 481, 24), // "on_btn_set_speed_clicked"
QT_MOC_LITERAL(24, 506, 35), // "on_btn_stop_x_turntable_run_c..."
QT_MOC_LITERAL(25, 542, 35), // "on_btn_stop_y_turntable_run_c..."
QT_MOC_LITERAL(26, 578, 24), // "on_btn_set_x_pos_clicked"
QT_MOC_LITERAL(27, 603, 24), // "on_btn_set_y_pos_clicked"
QT_MOC_LITERAL(28, 628, 21), // "on_btn_x_zero_clicked"
QT_MOC_LITERAL(29, 650, 21), // "on_btn_y_zero_clicked"
QT_MOC_LITERAL(30, 672, 32), // "on_pushButton_connection_clicked"
QT_MOC_LITERAL(31, 705, 35), // "on_pushButton_disconnection_c..."
QT_MOC_LITERAL(32, 741, 19), // "updateTurntableData"
QT_MOC_LITERAL(33, 761, 29), // "on_btn_set_target_pos_clicked"
QT_MOC_LITERAL(34, 791, 42), // "on_btn_set_pidcontroller_para..."
QT_MOC_LITERAL(35, 834, 14), // "closedLoopTick"
QT_MOC_LITERAL(36, 849, 31) // "on_controller_selection_changed"

    },
    "MainWindow\0on_pushButton_connect_clicked\0"
    "\0on_pushButton_disconnect_clicked\0"
    "onSocketError\0QAbstractSocket::SocketError\0"
    "error\0on_pushButton_motor_control_clicked\0"
    "on_pushButton_find_optimal_clicked\0"
    "initializeUIWithConfig\0trajTypeChanged\0"
    "index\0onMotorDataReceived\0MotorData_t\0"
    "data\0onChannelDataReceived\0CurrentVPCh_t\0"
    "onOptResDataReceived\0OptResData_t\0"
    "on_pushButton_filenamedisplay_clicked\0"
    "on_pushButton_read_file_clicked\0"
    "on_listWidget_files_itemSelectionChanged\0"
    "update_turntable_image\0on_btn_set_speed_clicked\0"
    "on_btn_stop_x_turntable_run_clicked\0"
    "on_btn_stop_y_turntable_run_clicked\0"
    "on_btn_set_x_pos_clicked\0"
    "on_btn_set_y_pos_clicked\0on_btn_x_zero_clicked\0"
    "on_btn_y_zero_clicked\0"
    "on_pushButton_connection_clicked\0"
    "on_pushButton_disconnection_clicked\0"
    "updateTurntableData\0on_btn_set_target_pos_clicked\0"
    "on_btn_set_pidcontroller_parameter_clicked\0"
    "closedLoopTick\0on_controller_selection_changed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      28,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  154,    2, 0x08 /* Private */,
       3,    0,  155,    2, 0x08 /* Private */,
       4,    1,  156,    2, 0x08 /* Private */,
       7,    0,  159,    2, 0x08 /* Private */,
       8,    0,  160,    2, 0x08 /* Private */,
       9,    0,  161,    2, 0x08 /* Private */,
      10,    1,  162,    2, 0x08 /* Private */,
      12,    1,  165,    2, 0x08 /* Private */,
      15,    1,  168,    2, 0x08 /* Private */,
      17,    1,  171,    2, 0x08 /* Private */,
      19,    0,  174,    2, 0x08 /* Private */,
      20,    0,  175,    2, 0x08 /* Private */,
      21,    0,  176,    2, 0x08 /* Private */,
      22,    0,  177,    2, 0x08 /* Private */,
      23,    0,  178,    2, 0x08 /* Private */,
      24,    0,  179,    2, 0x08 /* Private */,
      25,    0,  180,    2, 0x08 /* Private */,
      26,    0,  181,    2, 0x08 /* Private */,
      27,    0,  182,    2, 0x08 /* Private */,
      28,    0,  183,    2, 0x08 /* Private */,
      29,    0,  184,    2, 0x08 /* Private */,
      30,    0,  185,    2, 0x08 /* Private */,
      31,    0,  186,    2, 0x08 /* Private */,
      32,    0,  187,    2, 0x08 /* Private */,
      33,    0,  188,    2, 0x08 /* Private */,
      34,    0,  189,    2, 0x08 /* Private */,
      35,    0,  190,    2, 0x08 /* Private */,
      36,    1,  191,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, 0x80000000 | 16,   14,
    QMetaType::Void, 0x80000000 | 18,   14,
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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   11,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_pushButton_connect_clicked(); break;
        case 1: _t->on_pushButton_disconnect_clicked(); break;
        case 2: _t->onSocketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 3: _t->on_pushButton_motor_control_clicked(); break;
        case 4: _t->on_pushButton_find_optimal_clicked(); break;
        case 5: _t->initializeUIWithConfig(); break;
        case 6: _t->trajTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->onMotorDataReceived((*reinterpret_cast< const MotorData_t(*)>(_a[1]))); break;
        case 8: _t->onChannelDataReceived((*reinterpret_cast< const CurrentVPCh_t(*)>(_a[1]))); break;
        case 9: _t->onOptResDataReceived((*reinterpret_cast< const OptResData_t(*)>(_a[1]))); break;
        case 10: _t->on_pushButton_filenamedisplay_clicked(); break;
        case 11: _t->on_pushButton_read_file_clicked(); break;
        case 12: _t->on_listWidget_files_itemSelectionChanged(); break;
        case 13: _t->update_turntable_image(); break;
        case 14: _t->on_btn_set_speed_clicked(); break;
        case 15: _t->on_btn_stop_x_turntable_run_clicked(); break;
        case 16: _t->on_btn_stop_y_turntable_run_clicked(); break;
        case 17: _t->on_btn_set_x_pos_clicked(); break;
        case 18: _t->on_btn_set_y_pos_clicked(); break;
        case 19: _t->on_btn_x_zero_clicked(); break;
        case 20: _t->on_btn_y_zero_clicked(); break;
        case 21: _t->on_pushButton_connection_clicked(); break;
        case 22: _t->on_pushButton_disconnection_clicked(); break;
        case 23: _t->updateTurntableData(); break;
        case 24: _t->on_btn_set_target_pos_clicked(); break;
        case 25: _t->on_btn_set_pidcontroller_parameter_clicked(); break;
        case 26: _t->closedLoopTick(); break;
        case 27: _t->on_controller_selection_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
