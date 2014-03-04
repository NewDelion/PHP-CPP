/**
 *  ClassBase.h
 *
 *  This is the base class of the "Class" class. This is an internal class that
 *  is used by the PHP-CPP library. But because the constructor is protected,
 *  you can not create any instances if this class yourself (and you are not
 *  supposed to do that either.
 *
 *  Further more, because this base class is a 'private' base of Class, all 
 *  features of it are normally also inaccessible.
 *
 *  In other words: it is not meant to be directly used by extension writers.
 *
 *  @copyright 2014 Copernica BV
 */

/**
 *  Set up namespace
 */
namespace Php {

/**
 *  Method signatures
 */
typedef void    (Base::*method_callback_0)();
typedef void    (Base::*method_callback_1)(Parameters &);
typedef Value   (Base::*method_callback_2)();
typedef Value   (Base::*method_callback_3)(Parameters &);

/**
 *  Forward declarations
 */
class Method;
class Member;

/**
 *  Class definition
 */
class ClassBase
{
protected:
    /**
     *  Protected constructor
     *  @param  classname   Class name
     *  @param  type        The class type
     */
    ClassBase(const char *classname, ClassType type = ClassType::Regular) : _name(classname), _type(type) {}
    
public:
    /**
     *  Copy constructor
     *  @param  that
     */
    ClassBase(const ClassBase &that) : 
        _name(that._name), 
        _type(that._type), 
        _methods(that._methods), 
        _members(that._members), 
        _entry(nullptr) {}

    /**
     *  Move constructor
     *  @param  that
     */
    ClassBase(ClassBase &&that) :
        _name(std::move(that._name)), 
        _type(that._type), 
        _methods(std::move(that._methods)), 
        _members(std::move(that._members)), 
        _entry(that._entry) 
    {
        // other entry are invalid now (not that it is used..., class objects are
        // only moved during extension setup, when the entry pointer has not yet
        // been allocated)
        that._entry = nullptr;
    }

    /**
     *  Destructor
     */
    virtual ~ClassBase();

    /**
     *  Construct a new instance of the object
     *  @param  value
     *  @return Base
     */
    Base* construct(const struct _zend_object_value &value)
    {
        // construct the base
        auto *result = construct();
        if (!result) return nullptr;
        
        // assign the zend object to it
        // @todo fix this
//        result->assign(value);
        
        // done
        return result;
    }

    /**
     *  Construct a clone of the object
     *  @param  orig
     *  @param  value
     *  @return Base
     */
    Base* clone(Base *orig, const struct _zend_object_value &value)
    {
        // construct the base
        auto *result = clone(orig);
        if (!result) return nullptr;
        
        // assign the zend object to it
        // @todo fix this
//        result->assign(value);
        
        // done
        return result;
    }
    
    /**
     *  Construct a new instance of the object
     *  @return Base
     */
    virtual Base* construct() = 0;
    
    /**
     *  Create a clone of an object
     *  @param  orig
     *  @return Base
     */
    virtual Base *clone(Base *orig) = 0;

    /**
     *  Initialize the class, given its name
     * 
     *  The module functions are registered on module startup, but classes are
     *  initialized afterwards. The Zend engine is a strange thing. Nevertheless,
     *  this means that this method is called after the module is already available.
     *  This function will inform the Zend engine about the existence of the
     *  class.
     * 
     *  @param  ns          Namespace name
     */
    void initialize(const std::string &ns);
    
protected:
    /**
     *  Add a method to the class
     *  
     *  The method will be accessible as one of the class methods in your PHP
     *  code. When the method is called, it will automatically be forwarded
     *  to the C++ implementation. The flags can be Php::Public, Php::Protected
     *  or Php::Private (using private methods can be useful if you for example
     *  want to make the __construct() function private). The access-modified
     *  flag can be bitwise combined with the flag Php::Final or Php::Abstract).
     * 
     *  @param  name        Name of the method
     *  @param  method      The actual method
     *  @param  flags       Optional flags
     *  @param  args        Description of the supported arguments
     */
    void method(const char *name, method_callback_0, int flags=0, const Arguments &args = {});
    void method(const char *name, method_callback_1, int flags=0, const Arguments &args = {});
    void method(const char *name, method_callback_2, int flags=0, const Arguments &args = {});
    void method(const char *name, method_callback_3, int flags=0, const Arguments &args = {});

    /**
     *  Add an abstract method to the class
     * 
     *  @param  name        Name of the method
     *  @param  flags       Optional flags (like public or protected)
     *  @param  args        Description of the supported arguments
     */
    void method(const char *name, int flags=0, const Arguments &args = {});

    /**
     *  Add a property to the class
     * 
     *  Every instance of this class will have this property. The property
     *  can be Php::Public, Php::Protected or Php::Private (altough setting
     *  private properties is odd as the implementation of the class is in CPP,
     *  so why use private properties while the whole implementation is already
     *  hidden)
     * 
     *  @param  name        Name of the property
     *  @param  value       Actual property value
     *  @param  flags       Optional flags
     */
    void property(const char *name, std::nullptr_t value, int flags = Php::Public);
    void property(const char *name, int16_t value, int flags = Php::Public);
    void property(const char *name, int32_t value, int flags = Php::Public);
    void property(const char *name, int64_t value, int flags = Php::Public);
    void property(const char *name, bool value, int flags = Php::Public);
    void property(const char *name, char value, int flags = Php::Public);
    void property(const char *name, const std::string &value, int flags = Php::Public);
    void property(const char *name, const char *value, int flags = Php::Public);
    void property(const char *name, double value, int flags = Php::Public);

    /**
     *  Add an implemented interface
     *  
     *  This can only be used to register interfaces that are already defined
     *  by Zend, and not for user space interface or custom extension interfaces.
     *  This is probably not so much of a problem, as this feature is mostly
     *  useful for interfaces like 'Countable', 'ArrayAccess', 'Iterator', et
     *  cetera. Interfaces defined in user space are in normal operations
     *  inaccessible (user space code normally runs after the extension has been 
     *  set up) - so we do not need a feature to set these.
     * 
     *  It does however make sense to support implementing extension-specific
     *  interface. We may add this feature in the future.
     * 
     *  @param  interface
     */
    void interface(struct _zend_class_entry *interface)
    {
        // register the interface
        _interfaces.push_back(interface);
    }

private:
    /**
     *  Retrieve an array of zend_function_entry objects that hold the 
     *  properties for each method. This method is called at extension
     *  startup time to register all methods.
     * 
     *  @param  classname       The class name
     *  @return zend_function_entry[]
     */
    const struct _zend_function_entry *entries();

    /**
     *  Name of the class
     *  @var    string
     */
    std::string _name;

    /**
     *  The comment for reflexion, with a stored pointer to ourselves
     *  @var    char*
     */
    char *_comment = nullptr;

    /**
     *  The class type (this can be values like Php::Abstract and Php::Final)
     *  @var    ClassType
     */
    ClassType _type = ClassType::Regular;

    /** 
     *  The class entry
     *  @var    zend_class_entry
     */
    struct _zend_class_entry *_entry = nullptr;

    /**
     *  Pointer to the entries
     *  @var    zend_function_entry[]
     */
    struct _zend_function_entry *_entries = nullptr;
    
    /**
     *  All class methods
     *  @var    std::list
     */
    std::list<std::shared_ptr<Method>> _methods;
    
    /**
     *  All class members (class properties)
     *  @var    std::list
     */
    std::list<std::shared_ptr<Member>> _members;
    
    /**
     *  All interfaces that are implemented
     *  @var    std::list
     */
    std::list<struct _zend_class_entry*> _interfaces;
    
};
    
/**
 *  End namespace
 */
}

