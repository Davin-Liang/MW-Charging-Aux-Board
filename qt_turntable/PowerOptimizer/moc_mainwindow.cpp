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
    QByteArrayData data[36];
    char stringdata0[818];
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
QT_MOC_LITERAL(4, 75, 17), // "onSocketConnected"
QT_MOC_LITERAL(5, 93, 20), // "onSocketDisconnected"
QT_MOC_LITERAL(6, 114, 17), // "onSocketReadyRead"
QT_MOC_LITERAL(7, 132, 13), // "onSocketError"
QT_MOC_LITERAL(8, 146, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(9, 175, 5), // "error"
QT_MOC_LITERAL(10, 181, 35), // "on_pushButton_motor_control_c..."
QT_MOC_LITERAL(11, 217, 34), // "on_pushButton_find_optimal_cl..."
QT_MOC_LITERAL(12, 252, 22), // "initializeUIWithConfig"
QT_MOC_LITERAL(13, 275, 15), // "trajTypeChanged"
QT_MOC_LITERAL(14, 291, 5), // "index"
QT_MOC_LITERAL(15, 297, 19), // "onMotorDataReceived"
QT_MOC_LITERAL(16, 317, 11), // "MotorData_t"
QT_MOC_LITERAL(17, 329, 4), // "data"
QT_MOC_LITERAL(18, 334, 21), // "onChannelDataReceived"
QT_MOC_LITERAL(19, 356, 13), // "CurrentVPCh_t"
QT_MOC_LITERAL(20, 370, 20), // "onOptResDataReceived"
QT_MOC_LITERAL(21, 391, 12), // "OptResData_t"
QT_MOC_LITERAL(22, 404, 37), // "on_pushButton_filenamedisplay..."
QT_MOC_LITERAL(23, 442, 31), // "on_pushButton_read_file_clicked"
QT_MOC_LITERAL(24, 474, 40), // "on_listWidget_files_itemSelec..."
QT_MOC_LITERAL(25, 515, 22), // "update_turntable_image"
QT_MOC_LITERAL(26, 538, 24), // "on_btn_set_speed_clicked"
QT_MOC_LITERAL(27, 563, 35), // "on_btn_stop_x_turntable_run_c..."
QT_MOC_LITERAL(28, 599, 35), // "on_btn_stop_y_turntable_run_c..."
QT_MOC_LITERAL(29, 635, 24), // "on_btn_set_x_pos_clicked"
QT_MOC_LITERAL(30, 660, 24), // "on_btn_set_y_pos_clicked"
QT_MOC_LITERAL(31, 685, 21), // "on_btn_x_zero_clicked"
QT_MOC_LITERAL(32, 707, 21), // "on_btn_y_zero_clicked"
QT_MOC_LITERAL(33, 729, 32), // "on_pushButton_connection_clicked"
QT_MOC_LITERAL(34, 762, 35), // "on_pushButton_disconnection_c..."
QT_MOC_LITERAL(35, 798, 19) // "updateTurntableData"

    },
    "MainWindow\0on_pushButton_connect_clicked\0"
    "\0on_pushButton_disconnect_clicked\0"
    "onSocketConnected\0onSocketDisconnected\0"
    "onSocketReadyRead\0onSocketError\0"
    "QAbstractSocket::SocketError\0error\0"
    "on_pushButton_motor_control_clicked\0"
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
    "updateTurntableData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  149,    2, 0x08 /* Private */,
       3,    0,  150,    2, 0x08 /* Private */,
       4,    0,  151,    2, 0x08 /* Private */,
       5,    0,  152,    2, 0x08 /* Private */,
       6,    0,  153,    2, 0x08 /* Private */,
       7,    1,  154,    2, 0x08 /* Private */,
      10,    0,  157,    2, 0x08 /* Private */,
      11,    0,  158,    2, 0x08 /* Private */,
      12,    0,  159,    2, 0x08 /* Private */,
      13,    1,  160,    2, 0x08 /* Private */,
      15,    1,  163,    2, 0x08 /* Private */,
      18,    1,  166,    2, 0x08 /* Private */,
      20,    1,  169,    2, 0x08 /* Private */,
      22,    0,  172,    2, 0x08 /* Private */,
      23,    0,  173,    2, 0x08 /* Private */,
      24,    0,  174,    2, 0x08 /* Private */,
      25,    0,  175,    2, 0x08 /* Private */,
      26,    0,  176,    2, 0x08 /* Private */,
      27,    0,  177,    2, 0x08 /* Private */,
      28,    0,  178,    2, 0x08 /* Private */,
      29,    0,  179,    2, 0x08 /* Private */,
      30,    0,  180,    2, 0x08 /* Private */,
      31,    0,  181,    2, 0x08 /* Private */,
      32,    0,  182,    2, 0x08 /* Private */,
      33,    0,  183,    2, 0x08 /* Private */,
      34,    0,  184,    2, 0x08 /* Private */,
      35,    0,  185,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 19,   17,
    QMetaType::Void, 0x80000000 | 21,   17,
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
        case 2: _t->onSocketConnected(); break;
        case 3: _t->onSocketDisconnected(); break;
        case 4: _t->onSocketReadyRead(); break;
        case 5: _t->onSocketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 6: _t->on_pushButton_motor_control_clicked(); break;
        case 7: _t->on_pushButton_find_optimal_clicked(); break;
        case 8: _t->initializeUIWithConfig(); break;
        case 9: _t->trajTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->onMotorDataReceived((*reinterpret_cast< const MotorData_t(*)>(_a[1]))); break;
        case 11: _t->onChannelDataReceived((*reinterpret_cast< const CurrentVPCh_t(*)>(_a[1]))); break;
        case 12: _t->onOptResDataReceived((*reinterpret_cast< const OptResData_t(*)>(_a[1]))); break;
        case 13: _t->on_pushButton_filenamedisplay_clicked(); break;
        case 14: _t->on_pushButton_read_file_clicked(); break;
        case 15: _t->on_listWidget_files_itemSelectionChanged(); break;
        case 16: _t->update_turntable_image(); break;
        case 17: _t->on_btn_set_speed_clicked(); break;
        case 18: _t->on_btn_stop_x_turntable_run_clicked(); break;
        case 19: _t->on_btn_stop_y_turntable_run_clicked(); break;
        case 20: _t->on_btn_set_x_pos_clicked(); break;
        case 21: _t->on_btn_set_y_pos_clicked(); break;
        case 22: _t->on_btn_x_zero_clicked(); break;
        case 23: _t->on_btn_y_zero_clicked(); break;
        case 24: _t->on_pushButton_connection_clicked(); break;
        case 25: _t->on_pushButton_disconnection_clicked(); break;
        case 26: _t->updateTurntableData(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
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
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
