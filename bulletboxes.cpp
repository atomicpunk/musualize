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

#define GROUND_HEIGHT 0.f
#define OBJECT_HALFX 1
#define OBJECT_HALFY 1
#define OBJECT_HALFZ 1

#include <stdio.h>
#include "bulletboxes.h"
#include "GlutStuff.h"
#include "btBulletDynamicsCommon.h"

BulletBoxes::BulletBoxes()
:m_ccdMode(USE_CCD),m_columns(10),m_rows(10),m_levels(10),m_shape(SHAPE_BOX),m_randomize(true)
{
    setDebugMode(btIDebugDraw::DBG_DrawText+btIDebugDraw::DBG_NoHelpText);
    setCameraDistance(btScalar(40.));
}

void BulletBoxes::clientMoveAndDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    if (m_dynamicsWorld)
    {
        m_dynamicsWorld->stepSimulation(1./60.,0);//ms / 1000000.f);
    }
    renderme();
    glFlush();
    swapBuffers();
}

void BulletBoxes::displayCallback(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    renderme();
    glFlush();
    swapBuffers();
}

void    BulletBoxes::initPhysics()
{
    setTexturing(true);
    setShadows(true);
    m_ShootBoxInitialSpeed = 4000.f;

    m_defaultContactProcessingThreshold = 0.f;

    ///collision configuration contains default setup for memory, collision setup
    m_collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    m_dispatcher = new    btCollisionDispatcher(m_collisionConfiguration);
    //m_dispatcher->registerCollisionCreateFunc(BOX_SHAPE_PROXYTYPE,BOX_SHAPE_PROXYTYPE,m_collisionConfiguration->getCollisionAlgorithmCreateFunc(CONVEX_SHAPE_PROXYTYPE,CONVEX_SHAPE_PROXYTYPE));

    m_broadphase = new btDbvtBroadphase();

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
    m_solver = sol;

    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
    m_dynamicsWorld->getSolverInfo().m_solverMode |=SOLVER_USE_2_FRICTION_DIRECTIONS|SOLVER_RANDMIZE_ORDER;

    if (m_ccdMode==USE_CCD)
    {
        m_dynamicsWorld->getDispatchInfo().m_useContinuous=true;
    } else
    {
        m_dynamicsWorld->getDispatchInfo().m_useContinuous=false;
    }

    m_dynamicsWorld->setGravity(btVector3(0,-10,0));

    // create the ground shape
#if 0
    /* a floating rectangular box with GROUND_HEIGHT thickness in Y axis */
    btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(110.),btScalar(GROUND_HEIGHT),btScalar(110.)));
#else
    /* transparent 2D plane, X/Z plane of infinite size */
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),GROUND_HEIGHT);
#endif
    m_collisionShapes.push_back(groundShape);

    switch(m_shape)
    {
    case SHAPE_SPHERE:
        m_collisionShapes.push_back(new btSphereShape (btScalar(OBJECT_HALFY)));
        break;
    case SHAPE_CYLINDER:
        m_collisionShapes.push_back(new btCylinderShape (btVector3(OBJECT_HALFX,OBJECT_HALFY,OBJECT_HALFZ)));
        break;
    default:
        m_collisionShapes.push_back(new btBoxShape (btVector3(OBJECT_HALFX,OBJECT_HALFY,OBJECT_HALFZ)));
    }

    btTransform groundTransform;
    groundTransform.setIdentity();
    //groundTransform.setOrigin(btVector3(5,5,5));

    // Create the ground
    {
        // if non zero, the ground falls with the boxes
        btScalar mass(0.);
        bool isDynamic = (mass != 0.f);

        btVector3 localInertia(0,0,0);
        if (isDynamic)
            groundShape->calculateLocalInertia(mass,localInertia);

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
        btRigidBody* body = new btRigidBody(rbInfo);
        body->setFriction(0.5);
        //body->setRollingFriction(0.3);
        //add the body to the dynamics world
        m_dynamicsWorld->addRigidBody(body);
    }

    {
        //create a few dynamic rigidbodies
        // Re-using the same collision is better for memory usage and performance

        btCollisionShape* colShape = new btBoxShape(btVector3(OBJECT_HALFX,OBJECT_HALFY,OBJECT_HALFZ));

        //btCollisionShape* colShape = new btSphereShape(btScalar(1.));
        m_collisionShapes.push_back(colShape);

        /// Create Dynamic Objects
        btTransform startTransform;
        startTransform.setIdentity();

        btScalar    mass(1.f);

        //rigidbody is dynamic if and only if mass is non zero, otherwise static
        bool isDynamic = (mass != 0.f);

        btVector3 localInertia(0,0,0);
        if (isDynamic)
            colShape->calculateLocalInertia(mass,localInertia);

        if(m_randomize)
        {
            m_columns = 1 + (rand() % 10);
            m_rows = 1 + (rand() % 10);
            m_levels = (10 + (rand() % 91));
        }
        int gNumObjects = m_columns*m_rows*m_levels;
        printf("COLS=%d, ROWS=%d, LEVELS=%d, TOTAL=%d\n",
            m_columns, m_rows, m_levels, gNumObjects);

        for (int i = 0; i < gNumObjects; i++)
        {
            btCollisionShape* shape = m_collisionShapes[1];
            btTransform trans;
            trans.setIdentity();

            //stack them
            int level = i/(m_columns*m_rows);
            int col = (i)%(m_columns)-m_columns/2;
            int row = (i/m_columns)%(m_rows)-m_rows/2;

                        int x = col*2*OBJECT_HALFX;
                        int z = row*2*OBJECT_HALFZ;
                        int y = level*2*OBJECT_HALFY+GROUND_HEIGHT;
            btVector3 pos(x, y, z);

            trans.setOrigin(pos);
            float mass = 1.f;

            btRigidBody* body = localCreateRigidBody(mass,trans,shape);
            body->setAnisotropicFriction(shape->getAnisotropicRollingFrictionDirection(),btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
            body->setFriction(0.5);

            //body->setRollingFriction(.3);
            ///when using m_ccdMode
            if (m_ccdMode==USE_CCD)
            {
                body->setCcdMotionThreshold(OBJECT_HALFX);
                body->setCcdSweptSphereRadius(0.9*OBJECT_HALFX);
            }
        }
    }
    m_randomize = true;
}

void BulletBoxes::clientResetScene()
{
    exitPhysics();
    initPhysics();
}

void BulletBoxes::keyboardCallback(unsigned char key, int x, int y)
{
    if (key=='p')
    {
        switch (m_ccdMode)
        {
        case USE_CCD:
            m_ccdMode = USE_NO_CCD;
            break;
        case USE_NO_CCD:
        default:
            m_ccdMode = USE_CCD;
        };
        clientResetScene();
    }
    else if (key=='s')
    {
        m_shape = (m_shape+1)%SHAPE_NUM;
        m_randomize = false;
        clientResetScene();
    }
    else
    {
        DemoApplication::keyboardCallback(key,x,y);
    }
}

void BulletBoxes::shootBox(const btVector3& destination)
{

    if (m_dynamicsWorld)
    {
        float mass = 1.f;
        btTransform startTransform;
        startTransform.setIdentity();
        btVector3 camPos = getCameraPosition();
        startTransform.setOrigin(camPos);

        setShootBoxShape ();


        btRigidBody* body = this->localCreateRigidBody(mass, startTransform,m_shootBoxShape);
        body->setLinearFactor(btVector3(1,1,1));
        //body->setRestitution(1);

        btVector3 linVel(destination[0]-camPos[0],destination[1]-camPos[1],destination[2]-camPos[2]);
        linVel.normalize();
        linVel*=m_ShootBoxInitialSpeed;

        body->getWorldTransform().setOrigin(camPos);
        body->getWorldTransform().setRotation(btQuaternion(0,0,0,1));
        body->setLinearVelocity(linVel);
        body->setAngularVelocity(btVector3(0,0,0));
        body->setContactProcessingThreshold(1e30);

        ///when using m_ccdMode, disable regular CCD
        if (m_ccdMode==USE_CCD)
        {
            body->setCcdMotionThreshold(OBJECT_HALFX);
            body->setCcdSweptSphereRadius(0.4f);
        }
    }
}

void BulletBoxes::exitPhysics()
{

    //cleanup in the reverse order of creation/initialization

    //remove the rigidbodies from the dynamics world and delete them
    int i;
    for (i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
    {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        m_dynamicsWorld->removeCollisionObject( obj );
        delete obj;
    }

    //delete collision shapes
    for (int j=0;j<m_collisionShapes.size();j++)
    {
        btCollisionShape* shape = m_collisionShapes[j];
        delete shape;
    }
    m_collisionShapes.clear();

    delete m_dynamicsWorld;
    delete m_solver;
    delete m_broadphase;
    delete m_dispatcher;
    delete m_collisionConfiguration;
}
