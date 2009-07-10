#ifndef FenceActor_h__
#define FenceActor_h__

#include <dtDAL/plugin_export.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtABC/export.h>
#include <dtDAL/actorproperty.h>
#include <osgText/Text>
#include <dtActors/linkedpointsactorproxy.h>
#include <dtCore/refptr.h>
#include <osg/ref_ptr>
#include <dtCore/object.h>
#include <osg/Geode>
#include <osg/Geometry>

namespace dtDAL
{
   class ResourceDescriptor;
}

////////////////////////////////////////////////////////////////////////////////
// FENCE POST TRANSFORMABLE
////////////////////////////////////////////////////////////////////////////////
class FencePostTransformable : public dtCore::Transformable
{
private:

   struct GeomData
   {
      // Post
      dtCore::RefPtr<dtCore::Object> post;

      // Segment
      osg::ref_ptr<osg::Geometry>   segGeom;
      osg::ref_ptr<osg::Geode>      segGeode;

      osg::ref_ptr<osg::Vec3Array>  segVertexList;
      osg::ref_ptr<osg::Vec2Array>  segTextureList;
      osg::ref_ptr<osg::Vec3Array>  segNormalList;

      osg::ref_ptr<osg::Texture2D>  segTexture;
   };

   /**
   * Default Constructor.
   */
   FencePostTransformable();

   /**
   * Sets the current segment/post index.
   * This will ensure the arrays are large enough
   * for the given index but will not shrink the arrays
   * for a smaller index.
   *
   * @param[in]  index  The index to set.
   *
   * @return     Success or failure.
   */
   bool SetIndex(int index);

   /**
   * This will set the current size of the segment/post arrays.
   * Should be used when you know exactly how many posts and
   * segments there will be.
   *
   * @param[in]  size  The new size.
   *
   * @return     Success or failure.
   */
   bool SetSize(int size);

   /**
   * This will clear all parent geometry if the
   * geometry has been placed there by the parent.
   */
   void ClearParentGeometry();

protected:
   ~FencePostTransformable();

private:

   friend class FenceActor;

   std::vector<GeomData> mGeomList;

   dtCore::RefPtr<dtCore::Transformable> mOrigin;
   osg::ref_ptr<osg::Vec4Array>          mSegColorList;

   bool mRenderingParent;
};

/////////////////////////////////////////////////////////////////////////////
// ACTOR CODE
/////////////////////////////////////////////////////////////////////////////
class DT_PLUGIN_EXPORT FenceActor : public dtActors::LinkedPointsActor
{
public:
   typedef dtActors::LinkedPointsActor BaseClass;

   struct ResourceIDData
   {
      ResourceIDData()
      {
         postID = 0;
         segmentID = 0;
      }

      int      postID;
      int      segmentID;
   };

   FenceActor(dtDAL::ActorProxy* proxy, const std::string& name = "FenceActor");

   /**
   * This will visualize the current actor.
   */
   virtual void Visualize(void);

   /**
   * This will update the visuals on a single point.
   *
   * @param[in]  pointIndex  The point to update.
   */
   virtual void Visualize(int pointIndex);

   /**
   * Retrieves the list of fence post resources.
   */
   std::vector<std::string> GetPostResourceArray() const;

   /**
   * Functor to set the current post resource array.
   *
   * @param[in]  value  The new array.
   */
   void SetPostResourceArray(const std::vector<std::string>& value);

   /**
   * Retrieves the list of fence post resources.
   */
   std::vector<std::string> GetSegmentResourceArray() const;

   /**
   * Functor to set the current post resource array.
   *
   * @param[in]  value  The new array.
   */
   void SetSegmentResourceArray(const std::vector<std::string>& value);

   /**
   * Retrieves the list of fence post mesh indexes.
   */
   std::vector<std::vector<FenceActor::ResourceIDData> > GetResourceIDArray();

   /**
   * Sets the list of fence post mesh indexes.
   */
   void SetResourceIDArray(const std::vector<std::vector<FenceActor::ResourceIDData> >& value);

   /**
   * Retrieves the post mesh resource of a particular post.
   *
   * @param[in]  pointIndex  The index of the point.
   * @param[in]  subIndex    The index of the sub post between two points.
   *
   * @return     The name of the post mesh resource.  Empty if no meshes are available.
   */
   std::string GetPostMesh(int pointIndex, int subIndex);

   /**
   * Sets the post mesh resource for a particular post.
   *
   * @param[in]  pointIndex  The index of the point.
   * @param[in]  subIndex    The index of the sub post between two points.
   * @param[in]  meshIndex   The index of the mesh.
   */
   void SetPostMesh(int pointIndex, int subIndex, int meshIndex);

   /**
   * Gets the minimum distance between posts.
   */
   float GetPostMinDistance() { return mPostMinDistance; }

   /**
   * Sets the minimum distance between posts.
   */
   void SetPostMinDistance(float value);

   /**
   * Gets the maximum distance between posts.
   */
   float GetPostMaxDistance() { return mPostMaxDistance; }

   /**
   * Sets the maximum distance between posts.
   */
   void SetPostMaxDistance(float value);

   /**
   * Gets the post scale.
   */
   float GetFenceScale() { return mFenceScale; }

   /**
   * Sets the post scale.
   */
   void SetFenceScale(float value);

   /**
   * Retrieves the segment texture resource for a particular segment.
   *
   * @param[in]  pointIndex  The index of the point.
   * @param[in]  subIndex    The index of the sub post between two points.
   *
   * @return     The name of the segment texture resource.  Empty if no textures are available.
   */
   std::string GetSegmentTexture(int pointIndex, int subIndex);

   /**
   * Gets the height of the segments.
   */
   float GetSegmentHeight() { return mSegmentHeight; }

   /**
   * Sets the height of the segments.
   */
   void SetSegmentHeight(float value);

   /**
   * Gets the width of the segments.
   */
   float GetSegmentWidth() { return mSegmentWidth; }

   /**
   * Sets the width of the segments.
   */
   void SetSegmentWidth(float value);

   /**
   * Gets the top texture ratio.
   */
   float GetTopTextureRatio() { return mTopTextureRatio; }

   /**
   * Sets the top texture ratio.
   */
   void SetTopTextureRatio(float value);


   /**
   * Gets the scale of the fence.
   */
   osg::Vec3 GetScale() const;

   /**
   * Sets the scale fo the fence.
   */
   void SetScale(const osg::Vec3& value);


   /**
   * Creates the drawable representation of a new link point.
   *
   * @param[in]  position  The position of the drawable.
   *
   * @return               The new drawable.
   */
   virtual dtCore::Transformable* CreatePointDrawable(osg::Vec3 position);

protected:
   virtual ~FenceActor();

private:

   /**
   * Resizes the number of indexes in the resource ID list.
   *
   * @param[in]  pointIndex  The new total number of points.
   */
   void SetResourceIDSize(int pointCount);

   /**
   * Resizes the number of indexes in the resource ID sub list.
   *
   * @param[in]  pointIndex  The index of the point.
   * @param[in]  subCount    The new total count of sub indexes to set.
   */
   void SetResourceIDSubSize(int pointIndex, int subCount);

   /**
   * Places a post.
   *
   * @param[in]  pointIndex  The parent point of the post.
   * @param[in]  subIndex    The post index.
   * @param[in]  position    The position to place the post.
   *
   * @return     The transform of the newly placed post.
   */
   dtCore::Transform PlacePost(int pointIndex, int subIndex, osg::Vec3 position);

   /**
   * Places a segment.
   *
   * @param[in]  pointIndex  The parent point of the segment.
   * @param[in]  subIndex    The segment index.
   * @param[in]  start       The start post transform.
   * @param[in]  end         The end post transform.
   * @param[in]  length      The percentage of the texture to show.
   */
   void PlaceSegment(int pointIndex, int subIndex, dtCore::Transform start, dtCore::Transform end, float length);

   std::vector<std::string>                              mPostResourceList;
   std::vector<std::string>                              mSegmentResourceList;
   std::vector<std::vector<FenceActor::ResourceIDData> > mResourceIDList;

   float mPostMinDistance;
   float mPostMaxDistance;
   float mFenceScale;

   float mSegmentHeight;
   float mSegmentWidth;

   float mTopTextureRatio;
};

/////////////////////////////////////////////////////////////////////////////
// PROXY CODE
/////////////////////////////////////////////////////////////////////////////
class DT_PLUGIN_EXPORT FenceActorProxy : public dtActors::LinkedPointsActorProxy
{
public:
   typedef dtActors::LinkedPointsActorProxy BaseClass;

   /**
   * Constructor.
   */
   FenceActorProxy();

   /**
   * Creates the default actor associated with this proxy.
   */
   void CreateActor();

   /**
   * Builds the property map.
   */
   void BuildPropertyMap();

   /**
   * Functor to set the current post index.
   *
   * @param[in]  index  The index to set.
   */
   void SetPostResourceIndex(int index);

   /**
   * Functor to set the current post index.
   *
   * @param[in]  index  The index to set.
   */
   void SetSegmentResourceIndex(int index);

   /**
   * Functor to retrieve the default value for a new post.
   */
   std::string GetDefaultPost(void);

   /**
   * Functor to retrieve the default value for a new post.
   */
   std::string GetDefaultSegment(void);

   /**
   * Gets a ResourceDescriptor of the requested property name.
   * @param name Name of the resource to retrieve.
   * @return A pointer to the resource descripter or NULL if it
   * is not found.
   */
   dtDAL::ResourceDescriptor* GetResource(const std::string& name);

   /**
   * Gets a ResourceDescriptor of the requested property name.
   * @param name Name of the resource to retrieve.
   * @return A pointer to the resource descripter or NULL if it
   * is not found.
   */
   const dtDAL::ResourceDescriptor* GetResource(const std::string& name) const;

   /**
   * Sets the mesh used as a fence post.
   */
   void SetPostMesh(const std::string& fileName);

   /**
   * Sets the texture used as a fence segment.
   */
   void SetSegmentTexture(const std::string& fileName);

   /**
   * Functor to set the current post ID index.
   *
   * @param[in]  index  The index to set.
   */
   void SetResourceIDIndex(int index);

   /**
   * Functor to set the current sub post ID index.
   *
   * @param[in]  index  The index to set.
   */
   void SetResourceIDSubIndex(int index);

   /**
   * Functor to retrieve the default value for a post ID list.
   */
   std::vector<FenceActor::ResourceIDData> GetDefaultResourceID();

   /**
   * Functor to retrieve the default value for a sub post ID.
   */
   FenceActor::ResourceIDData GetDefaultSubResourceID();

   /**
   * Retrieves the sub post ID array.
   */
   std::vector<FenceActor::ResourceIDData> GetSubResourceIDArray();

   /**
   * Sets the sub post ID array.
   */
   void SetSubResourceIDArray(const std::vector<FenceActor::ResourceIDData>& value);

   /**
   * Gets and Sets the post IDs.
   */
   int GetPostID();
   void SetPostID(int value);

   /**
   * Gets and Sets the segment IDs.
   */
   int GetSegmentID();
   void SetSegmentID(int value);

protected:
   virtual ~FenceActorProxy();

private:

   int      mPostResourceIndex;
   int      mSegmentResourceIndex;

   int      mResourceIDIndex;
   int      mResourceIDSubIndex;
};

#endif // FenceActor_h__
