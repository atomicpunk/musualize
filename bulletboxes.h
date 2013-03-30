/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
#ifndef BULLETBOXES_H
#define BULLETBOXES_H

#include "GlutDemoApplication.h"
#include "LinearMath/btAlignedObjectArray.h"

class btBroadphaseInterface;
class btCollisionShape;
class btOverlappingPairCache;
class btCollisionDispatcher;
class btConstraintSolver;
struct btCollisionAlgorithmCreateFunc;
class btDefaultCollisionConfiguration;

class BulletBoxes : public GlutDemoApplication
{
	//keep the collision shapes, for deletion/cleanup
	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
	btBroadphaseInterface*	m_broadphase;
	btCollisionDispatcher*	m_dispatcher;
	btConstraintSolver*	m_solver;

	enum
	{
		USE_CCD=1,
		USE_NO_CCD
	};
	enum
	{
		SHAPE_BOX=0,
		SHAPE_CYLINDER,
		SHAPE_SPHERE,
		SHAPE_NUM
	};
	int m_ccdMode;
        int m_columns;
        int m_rows;
        int m_levels;
        int m_shape;
	bool m_randomize;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
public:
	BulletBoxes();

	virtual ~BulletBoxes()
	{
		exitPhysics();
	}
	void	initPhysics();

	void	exitPhysics();

	virtual void clientMoveAndDisplay();

	virtual void keyboardCallback(unsigned char key, int x, int y);

	virtual void displayCallback();
	virtual void shootBox(const btVector3& destination);
	virtual void clientResetScene();

	static DemoApplication* Create()
	{
		BulletBoxes* demo = new BulletBoxes;
		demo->myinit();
		demo->initPhysics();
		return demo;
	}

};

#endif //BULLETBOXES_H

