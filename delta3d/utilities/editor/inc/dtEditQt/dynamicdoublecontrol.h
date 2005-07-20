/* 
* Delta3D Open Source Game and Simulation Engine Level Editor 
* Copyright (C) 2005, BMH Associates, Inc. 
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free 
* Software Foundation; either version 2 of the License, or (at your option) 
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
* details.
*
* You should have received a copy of the GNU General Public License 
* along with this library; if not, write to the Free Software Foundation, Inc., 
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
*
* @author Curtiss Murphy
*/
#ifndef DELTA_DYNAMICDOUBLECONTROL
#define DELTA_DYNAMICDOUBLECONTROL

#include "dtEditQt/dynamicabstractcontrol.h"
#include "dtEditQt/dynamicsubwidgets.h"

class QWidget;

namespace dtDAL 
{
    class DoubleActorProperty;
}

namespace dtEditQt 
{

    /**
    * @class DynamicDoubleControl
    * @brief This is the dynamic control for the double data type - used in the property editor
    * @note This is almost identical to the float control, but has different data types.
    */
    class DynamicDoubleControl : public DynamicAbstractControl
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        DynamicDoubleControl();

        /**
         * Destructor
         */
        virtual ~DynamicDoubleControl();

        /**
         * @see DynamicAbstractControl#initializeData
         */
        virtual void initializeData(DynamicAbstractControl *newParent, PropertyEditorModel *model,
            dtDAL::ActorProxy *proxy, dtDAL::ActorProperty *property);

        /**
         * @see DynamicAbstractControl#updateEditorFromModel
         */
        virtual void updateEditorFromModel(QWidget *widget);

        /**
         * @see DynamicAbstractControl#updateModelFromEditor
         */
        virtual bool updateModelFromEditor(QWidget *widget);

        /**
         * @see DynamicAbstractControl#createEditor
         */
        virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
            const QModelIndex &index);

        /**
         * @see DynamicAbstractControl#getDisplayName
         */
        virtual const QString getDisplayName();

         /**
         * @see DynamicAbstractControl#getDescription
         */
        virtual const QString getDescription();

         /**
         * @see DynamicAbstractControl#getValueAsString
         */
        virtual const QString getValueAsString();

         /**
         * @see DynamicAbstractControl#isEditable
         */
        virtual bool isEditable();

        /**
         * @see DynamicAbstractControl#handleSubEditDestroy
         */
        void handleSubEditDestroy(QWidget *widget)
        {
            // we have to check - sometimes the destructor won't get called before the 
            // next widget is created.  Then, when it is called, it sets the NEW editor to NULL!
            if (widget == temporaryEditControl)
                temporaryEditControl = NULL;
        }

   public slots: 

        /**
         * @see DynamicAbstractControl#updateData
         */
        virtual bool updateData(QWidget *widget);

        void actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
            osg::ref_ptr<dtDAL::ActorProperty> property);
    protected:

    private: 
        dtDAL::DoubleActorProperty *myProperty;

        // This pointer is not really in our control.  It is constructed in the createEditor() 
        // method and destroyed whenever QT feels like it (mostly when the control looses focus). 
        // We work around this by trapping the destruction of this object, it should
        // call our handleSubEditDestroy() method so we know to not hold this anymore
        SubQLineEdit *temporaryEditControl;
    };

}

#endif
