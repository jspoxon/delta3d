/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2007 MOVES Institute
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
* Michael Guerrero
*/

#ifndef __POSE_MESH_UTILITY_H__
#define __POSE_MESH_UTILITY_H__

#include "PoseMesh.h"

namespace dtAnim 
{ 
   class Cal3DModelWrapper; 

   /**
   * Convenience functionality for manipulating celestial meshes
   */
   class DT_ANIM_EXPORT PoseMeshUtility
   {
   public:

      PoseMeshUtility();
      ~PoseMeshUtility();

      typedef std::pair<int, float> BaseReferencePose;

      struct BaseReferenceBlend
      {
         int startAnimID;
         int endAnimID;

         osg::Vec3 startDirection;
         osg::Vec3 endDirection;

         float blendAlpha;
      };

      void ClearPoses( PoseMesh *poseMesh, dtAnim::Cal3DModelWrapper *model, float delay );

      void BlendPoses( float azimuth, 
         float elevation, 
         PoseMesh::TargetTriangle &targetTriangle,
         PoseMesh *poseMesh,
         dtAnim::Cal3DModelWrapper* model );

      void SetBaseReferencePoses( std::vector<BaseReferencePose> *poseList,
         dtAnim::Cal3DModelWrapper* model );

      bool GetBaseReferenceBlend( float overallAlpha, BaseReferenceBlend &finalBlend );

   private:

      std::vector<BaseReferencePose> *mPoseList;
      std::vector<osg::Vec3> mPoseDirections;

      int GetEndPoseIndex( float alpha );

      bool AreBaseReferencePosesValid( const std::vector<BaseReferencePose> *poseList );
   };
}

#endif // __POSE_MESH_UTILITY_H__
