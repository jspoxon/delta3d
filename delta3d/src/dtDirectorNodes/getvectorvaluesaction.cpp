/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Author: Eric R. Heine
 */

#include <dtDirectorNodes/getvectorvaluesaction.h>

#include <dtDAL/stringactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   GetVectorValuesAction::GetVectorValuesAction()
      : ActionNode()
   {
      AddAuthor("Eric R. Heine");
   }

   /////////////////////////////////////////////////////////////////////////////
   GetVectorValuesAction::~GetVectorValuesAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetVectorValuesAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetVectorValuesAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::StringActorProperty* vectorProp = new dtDAL::StringActorProperty(
         "Vector", "Vector",
         dtDAL::StringActorProperty::SetFuncType(this, &GetVectorValuesAction::SetVector),
         dtDAL::StringActorProperty::GetFuncType(this, &GetVectorValuesAction::GetVector),
         "The vector to extract values from.");

      mXProp = new dtDAL::StringActorProperty(
         "X", "X",
         dtDAL::StringActorProperty::SetFuncType(this, &GetVectorValuesAction::SetX),
         dtDAL::StringActorProperty::GetFuncType(this, &GetVectorValuesAction::GetX),
         "The x value of the vector.");

      mYProp = new dtDAL::StringActorProperty(
         "Y", "Y",
         dtDAL::StringActorProperty::SetFuncType(this, &GetVectorValuesAction::SetY),
         dtDAL::StringActorProperty::GetFuncType(this, &GetVectorValuesAction::GetY),
         "The y value of the vector.");

      mZProp = new dtDAL::StringActorProperty(
         "Z", "Z",
         dtDAL::StringActorProperty::SetFuncType(this, &GetVectorValuesAction::SetZ),
         dtDAL::StringActorProperty::GetFuncType(this, &GetVectorValuesAction::GetZ),
         "The z value of the vector.");

      mWProp = new dtDAL::StringActorProperty(
         "W", "W",
         dtDAL::StringActorProperty::SetFuncType(this, &GetVectorValuesAction::SetW),
         dtDAL::StringActorProperty::GetFuncType(this, &GetVectorValuesAction::GetW),
         "The w value of the vector.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, vectorProp, false, false, false));
      mValues.push_back(ValueLink(this, mXProp.get(), true, true, false));
      mValues.push_back(ValueLink(this, mYProp.get(), true, true, false));
      mValues.push_back(ValueLink(this, mZProp.get(), true, true, false));
      mValues.push_back(ValueLink(this, mWProp.get(), true, true, false));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GetVectorValuesAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      dtDAL::DataType& vecType = GetPropertyType("Vector");

      osg::Vec4 result;
      if (vecType == dtDAL::DataType::VEC2F)
      {
         osg::Vec2 vec2A = GetVec2("Vector");
         result.x() = vec2A.x();
         result.y() = vec2A.y();
         result.z() = 0;
         result.w() = 0;
      }
      else if (vecType == dtDAL::DataType::VEC3F)
      {
         osg::Vec3 vec3A = GetVec3("Vector");
         result.x() = vec3A.x();
         result.y() = vec3A.y();
         result.z() = vec3A.z();
         result.w() = 0;
      }
      else if (vecType == dtDAL::DataType::VEC4F)
      {
         result = GetVec4("Vector");
      }

      SetString(dtUtil::ToString(result.x()), "X");
      SetString(dtUtil::ToString(result.y()), "Y");
      SetString(dtUtil::ToString(result.z()), "Z");
      SetString(dtUtil::ToString(result.w()), "W");

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool GetVectorValuesAction::CanConnectValue(ValueLink* link, ValueNode* value)
   {
      if (Node::CanConnectValue(link, value))
      {
         // Result checks its own type
         if (link->GetName() == "Vector")
         {
            dtDAL::DataType& type = value->GetPropertyType();
            switch (type.GetTypeId())
            {
            case dtDAL::DataType::VEC2F_ID:
            case dtDAL::DataType::VEC3F_ID:
            case dtDAL::DataType::VEC4F_ID:
               return true;

            default:
               return false;
            }
         }
         else
         {
            dtDAL::DataType& type = value->GetPropertyType();
            switch (type.GetTypeId())
            {
            case dtDAL::DataType::INT_ID:
            case dtDAL::DataType::FLOAT_ID:
            case dtDAL::DataType::DOUBLE_ID:
               return true;

            default:
               return false;
            }
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GetVectorValuesAction::SetVector(const std::string& value)
   {
      mVector = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string GetVectorValuesAction::GetVector() const
   {
      return mVector;
   }

   //////////////////////////////////////////////////////////////////////////
   void GetVectorValuesAction::SetX(const std::string& value)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   std::string GetVectorValuesAction::GetX() const
   {
      return "";
   }

   //////////////////////////////////////////////////////////////////////////
   void GetVectorValuesAction::SetY(const std::string& value)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   std::string GetVectorValuesAction::GetY() const
   {
      return "";
   }

   //////////////////////////////////////////////////////////////////////////
   void GetVectorValuesAction::SetZ(const std::string& value)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   std::string GetVectorValuesAction::GetZ() const
   {
      return "";
   }

   //////////////////////////////////////////////////////////////////////////
   void GetVectorValuesAction::SetW(const std::string& value)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   std::string GetVectorValuesAction::GetW() const
   {
      return "";
   }
}

////////////////////////////////////////////////////////////////////////////////