/****************************************************************************
** Meta object code from reading C++ file 'tabs.hpp'
**
** Created: Sat Jan 19 17:13:19 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "tabs.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tabs.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Tabs[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       6,    5,    5,    5, 0x08,
      16,    5,    5,    5, 0x08,
      26,    5,    5,    5, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Tabs[] = {
    "Tabs\0\0restart()\0history()\0send()\0"
};

void Tabs::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Tabs *_t = static_cast<Tabs *>(_o);
        switch (_id) {
        case 0: _t->restart(); break;
        case 1: _t->history(); break;
        case 2: _t->send(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Tabs::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Tabs::staticMetaObject = {
    { &QTabWidget::staticMetaObject, qt_meta_stringdata_Tabs,
      qt_meta_data_Tabs, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Tabs::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Tabs::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Tabs::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Tabs))
        return static_cast<void*>(const_cast< Tabs*>(this));
    if (!strcmp(_clname, "Ui::tabs"))
        return static_cast< Ui::tabs*>(const_cast< Tabs*>(this));
    return QTabWidget::qt_metacast(_clname);
}

int Tabs::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
