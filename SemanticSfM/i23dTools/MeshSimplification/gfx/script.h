#ifndef GFXSCRIPT_INCLUDED // -*- C++ -*-
#define GFXSCRIPT_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Simple low-level file format scripting support.

  This is the THIRD major revision of this code.  It represents a
  significant departure from the previous scripting structure.
    
  $Id: script.h 443 2005-06-14 00:53:40Z garland $

 ************************************************************************/

#include "gfx.h"
#include <vector>
#include <string>
#include <map>
//#include <hash_map>

namespace gfx
{

// Define a set of exceptions that can occur during script parsing
//
    namespace script
    {
	struct Error
	{
	    std::string msg;
	    Error(const std::string& m) : msg(m) {}
	};

	struct SyntaxError : public Error
	    { SyntaxError(const std::string& m) : Error(m) {} };

	struct NameError : public Error
	    { NameError(const std::string& m) : Error(m) {} };

	struct IOError : public Error
	    { IOError(const std::string& m) : Error(m) {} };
    }


// These return codes are deprecated and will be going away in the near
// future.  Start using the exceptions defined above instead.
enum {
    SCRIPT_OK = 0,
    SCRIPT_ERR_UNDEF,
    SCRIPT_ERR_SYNTAX,
    SCRIPT_ERR_UNSUPPORTED,
    SCRIPT_ERR_NOFILE,
    SCRIPT_END
};

class CmdLine
{
public:
    typedef std::string::size_type index_type;
    typedef std::pair<index_type, index_type> range_type;

    const std::string &line;
    range_type op;
    std::vector<range_type> tokens;

    std::string substr(const range_type& r) const
	{ return line.substr(r.first, r.second-r.first);}

    std::string token_to_string(int i) const;
    double token_to_double(int i) const;
    float token_to_float(int i) const;
    int token_to_int(int i) const;
    std::string rest_to_string(int i) const;
    

    CmdLine(const std::string &l) : line(l) { }

    std::string opname() const { return substr(op); }
    int argcount() const { return tokens.size(); }
    std::string argline() const;

    int collect_as_strings(std::vector<std::string> &v, int offset=0) const;
    int collect_as_numbers(std::vector<double> &v, int offset=0) const;
    int collect_as_numbers(std::vector<int> &v, int offset=0) const;

    int collect_as_numbers(double *v, int size, int offset=0) const;
    int collect_as_numbers(float *v, int size, int offset=0) const;
    int collect_as_numbers(int *v, int size, int offset=0) const;
};

typedef int (*CmdHandler)(const CmdLine&);

struct CmdObject
{
    virtual ~CmdObject() {}
    virtual int operator()(const CmdLine& cmd) = 0;
};

struct CmdFunction : public CmdObject
{
    CmdHandler fn;
    CmdFunction(CmdHandler f) { fn=f; }
    virtual int operator()(const CmdLine& cmd) { return (*fn)(cmd); }
};

template<class T> struct CmdMethod : public CmdObject
{
    typedef int (T::*member_handler)(const CmdLine&);
    T *self;
    member_handler fn;

    CmdMethod(T &obj, member_handler p) { self=&obj; fn=p; }
    virtual int operator()(const CmdLine& cmd) { return (self->*fn)(cmd); }
};

template<class T> struct CmdMethod2 : public CmdObject
{
    typedef void (T::*member_handler)(const CmdLine&);
    T *self;
    member_handler fn;

    CmdMethod2(T &obj, member_handler p) { self=&obj; fn=p; }
    virtual int operator()(const CmdLine& cmd)
    {
	(self->*fn)(cmd);
	return SCRIPT_OK;
    }
};

//typedef std::hash_map< std::string, CmdHandler > CmdTable;
typedef std::map< std::string, CmdObject* > CmdTable;

class CmdEnv
{
private:
    CmdTable script_commands;

    int script_include(const CmdLine&);
    int script_ignore(const CmdLine&);
    int script_end(const CmdLine&);
    int script_eval(const CmdLine&);

    std::vector<CmdEnv*> scopes;

public:
    CmdEnv();
    virtual ~CmdEnv();

    void register_command(const std::string& name, CmdObject *fn);
    CmdObject *lookup_command(const std::string& name);

    void register_command(const std::string& name, CmdHandler proc);

    template<class T> inline void register_method(const std::string& name,
						  T *obj,
						  int (T::*fn)(const CmdLine&))
    { register_command(name, new CmdMethod<T>(*obj, fn)); }

    template<class T> inline void register_method(const std::string& name,
						  T *obj,
						  void (T::*fn)(const CmdLine&))
    { register_command(name, new CmdMethod2<T>(*obj, fn)); }

    void ignore_command(const std::string& name);

    void register_vocabulary(const std::string& name, CmdEnv *env);
    void begin_scope(CmdEnv *subenv);
    void end_scope();

    int do_line(const std::string& line);
    int do_stream(std::istream& in);
    int do_file(const std::string& filename);
    int do_string(const std::string& line);
};

} // namespace gfx

// GFXSCRIPT_INCLUDED
#endif
