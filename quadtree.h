#pragma once

#include "collide.h"

//#define USE_EMBREE //!! experimental, but working, collision detection replacement for our quad and kd-tree //!! picking in debug mode so far not implemented though
#ifdef USE_EMBREE
 #include "inc\embree3\rtcore.h"
#endif

class HitQuadtree
{
public:
   HitQuadtree()
   {
#ifndef USE_EMBREE
      m_unique = NULL;
      m_leaf = true;
      lefts_rights_tops_bottoms_zlows_zhighs = 0;
#else
      m_embree_device = rtcNewDevice(nullptr);
      m_scene = nullptr;
#endif
   }

   ~HitQuadtree();

   void AddElement(HitObject *pho) { m_vho.push_back(pho); }
   void Initialize(const FRect3D& bounds);

#ifdef USE_EMBREE
   void FillFromVector(vector<HitObject*>& vho);
   void Update();
#endif

#ifndef USE_EMBREE
   void HitTestBall(const Ball * const pball, CollisionEvent& coll) const;
#else
   void HitTestBall(std::vector<Ball*> ball) const;
#endif
   void HitTestXRay(const Ball * const pball, vector<HitObject*> &pvhoHit, CollisionEvent& coll) const;

private:

   void Initialize();

#ifndef USE_EMBREE
   void CreateNextLevel(const FRect3D& bounds, const unsigned int level, unsigned int level_empty);
   void HitTestBallSse(const Ball * const pball, CollisionEvent& coll) const;

   void* __restrict m_unique; // everything below/including this node shares the same original primitive/hittarget object (just for early outs if not collidable)

   HitQuadtree * __restrict m_children[4];
   Vertex3Ds m_vcenter;

   // helper arrays for SSE boundary checks
   void InitSseArrays();
   float* __restrict lefts_rights_tops_bottoms_zlows_zhighs;

   bool m_leaf;
   unsigned char m_ObjType; // only used if m_unique != NULL, to identify which object this is (eObjType)
#else
   std::vector<HitObject*> *m_pvho;

   RTCDevice m_embree_device; //!! have only one instead of the two (dynamic and non-dynamic)?
   RTCScene m_scene;
#endif

   std::vector<HitObject*> m_vho;

#if !defined(NDEBUG) && defined(PRINT_DEBUG_COLLISION_TREE)
public:
   void DumpTree(const int indentLevel)
   {
#ifndef USE_EMBREE
      char indent[256];
      for (int i = 0; i <= indentLevel; ++i)
         indent[i] = (i == indentLevel) ? 0 : ' ';
      char msg[256];
      sprintf_s(msg, "[%f %f], items=%u", m_vcenter.x, m_vcenter.y, m_vho.size());
      strncat_s(indent, msg, sizeof(indent)-strnlen_s(indent, sizeof(indent))-1);
      OutputDebugString(indent);
      if (!m_leaf)
      {
         m_children[0]->DumpTree(indentLevel + 1);
         m_children[1]->DumpTree(indentLevel + 1);
         m_children[2]->DumpTree(indentLevel + 1);
         m_children[3]->DumpTree(indentLevel + 1);
      }
#endif
   }
#endif
};
