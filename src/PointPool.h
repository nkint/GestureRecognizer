#ifndef POINTPOOL_H_
#define POINTPOOL_H_

#include <list>
#include <iostream>

#include "ofPoint.h"

// adapted from https://gist.github.com/1124832

class PointPool
{
    private:
        std::list<ofPoint*> resources;
        static PointPool* instance;

        PointPool() {}

    public:
        ~PointPool() {}
        /**
         * Static method for accessing class instance.
         * Part of Singleton design pattern.
         *
         * @return ObjectPool instance.
         */
        static PointPool* getInstance();

        /**
         * Returns instance of Resource.
         *
         * New resource will be created if all the resources
         * were used at the time of the request.
         *
         * @return Resource instance.
         */
        ofPoint* getPoint();


        /**
         * Return resource back to the pool.
         *
         * The resource must be initialized back to
         * the default settings before someone else
         * attempts to use it.
         *
         * @param object Resource instance.
         * @return void
         */
        void disposePoint(ofPoint* object);
};


#endif /* POINTPOOL_H_ */
