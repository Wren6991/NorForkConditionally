#ifndef RESOURCEP_H_INCLUDED
#define RESOURCEP_H_INCLUDED

// tiny RAII wrapper class.
// Instantiates a new T when it's instantiated.
// If it gets popped off the stack, the T gets destroyed.
// Call release() when it's safe to take control of the pointer yourself.

template <typename T>
struct resourcep
{
    T *obj;
    public:
    resourcep()
    {
        obj = new T();
    }
    ~resourcep()
    {
        if (obj)
            delete obj;
    }
    T* release()
    {
        T *handle = obj;
        object = 0;
        return handle;
    }
};


#endif // RESOURCEP_H_INCLUDED
