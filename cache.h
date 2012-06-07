//Header for cache handling

#ifndef _CACHE
#define _CACHE

#ifdef _ERROR//If the error handling header has been included
	int CACHE_ERROR 						= getErrorCode();//Code for generic cache errors
	int CACHE_WARNING						= getErrorCode();//Code for generic cache warnings
	int CACHE_WARNING_NOLOADFUNC			= getErrorCode();//No loading function template parameter warning
#endif

#include <string>
#include <deque>

//Cached object class
//Each cache object contains the object itself, which is of the type in the template, and its id
//The id will be used to get a pointer to that object and corresponds to the file path
//The loadFunc template argument is the function used to load the data member from a file
template <class type, type (*loadFunc)(const char*)> class cachedObj{
	public:
	string id;//Id of the cached object
	type data;//Data into the object
	
	//Constructor
	cachedObj (){
		id = "";
		
		#ifdef _ERROR
			if (errFunc && !loadFunc) errFunc(CACHE_WARNING_NOLOADFUNC, "No loading function template parameter passed to cached object");
		#endif
	}
	
	//Constructor
	cachedObj (string path){
		load(path);
		
		#ifdef _ERROR
			if (errFunc && !loadFunc) errFunc(CACHE_WARNING_NOLOADFUNC, "No loading function template parameter passed to cached object");
		#endif
	}
	
	//Loading function
	void load(string path){
		id = path;//Sets the id
		
		if (loadFunc)//If the loading function exists
			data = loadFunc(path.c_str());//Loads the data
			
		#ifdef _ERROR
			else if (errFunc) errFunc(CACHE_WARNING_NOLOADFUNC, "Trying to load cached object without loading function - no data loaded");
		#endif
	}
};

//Class cache
//Consists of a deque of cached objects and some functions to load them and obtain pointers
template <class type, type (*loadFunc)(const char*)> class cache{
	public:
	deque < cachedObj <type, loadFunc> > objects;//Objects loaded into cache
	
	//Constructor
	cache(){
		#ifdef _ERROR
			if (errFunc && !loadFunc) errFunc(CACHE_WARNING_NOLOADFUNC, "No loading function template parameter passed to cache");
		#endif
	}
	
	//Function to create object
	void loadObject(string id){
		cachedObj <type, loadFunc> newObj (id);//New object
		objects.push_back(newObj);//Adds the object
	}
	
	//Is-in-cache function
	bool isInCache(string id){
		typename deque< cachedObj <type, loadFunc> >::iterator i;//Iterator
		
		for (i = objects.begin(); i != objects.end(); i++)//For each object
			if (i->id == id) return true;//Returns true if the object matches the id
			
		return false;//Returns false
	}
	
	//Function to get object from cache
	//If the object doesn't exist, the function tries to load it and returns the newly loaded object
	type getObj(string id){
		typename deque< cachedObj <type, loadFunc> >::iterator i;//Iterator
		
		for (i = objects.begin(); i != objects.end(); i++)//For each object
			if (i->id == id) return i->data;//Returns the data of the object if it matches the id
			
		loadObject(id);//Loads the object
		return objects[objects.size() - 1].data;//Returns data of the last object
	}
};

#endif