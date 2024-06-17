#ifndef XINMJ_BASE_STRINGPIECE_H
#define XINMJ_BASE_STRINGPIECE_H

#include <xin/base/Types.h>

#include <iosfwd>
#include <string.h>
#ifndef XIN_STD_STRING
#include <string>
#endif//!XIN_STD_STRING

namespace xin
{

class StringArg
{
 public:
  StringArg(const char* str)
   : str_(str)
  { }

  StringArg(const string& str)
   : str_(str.c_str())
  { }

#ifndef XIN_STD_STRING
  StringArg(const std::string& str)
   : str_(str.c_str())
  { }
#endif//!XIN_STD_STRING

 const char* c_str() const { return str_; }
 private:
  const char* str_;
};

class StringPiece
{
 public:
  StringPiece() : str_(NULL),len_(0){ }
  StringPiece(const char* str)
   : str_(str),len_(static_cast<int>(strlen(str))){ }
  StringPiece(const unsigned char* str)
   : str_(reinterpret_cast<const char*>(str)),
     len_(static_cast<int>(strlen(str_))){ }
  StringPiece(const string& str)
   : str_(str.c_str()),len_(static_cast<int>(str.size())){ }
#ifndef XIN_STD_STRING
  StringPiece(const std::string& str)
   : str_(str.c_str()),len_(static_cast<int>(str.size())){ }
#endif
  StringPiece(const char* offset,int len)
   : str_(offset),len_(len){ }

  const char* data() const { return str_; }
  int size() const { return len_; }
  bool empty() const { return len_ == 0; }
  const char* begin() const { return str_; }
  const char* end() const { return str_ + len_; }

  void clear(){ str_ = NULL; len_ = 0; }
  void set(const char* buf,int len){ str_ = buf; len_ = len; }
  void set(const char* str){ str_ = str; len_ = static_cast<int>(strlen(str)); }
  void set(const void* buf,int len)
  { 
    str_ = reinterpret_cast<const char*>(buf);
    len_ = len;
  }

  char operator[](int i) const { return str_[i]; }

  void remove_prefix(int n)
  {
    str_ += n;
    len_ -= n;
  }

  void remove_suffix(int n)
  {
    len_ -= n;
  }

  bool operator==(const StringPiece& other)
  {
    return (other.len_ == len_ && memcmp(str_,other.str_,len_) == 0);
  }

  bool operator!=(const StringPiece& other)
  {
    return !(*this == other);
  }

#define STRINGPIECE_BINARY_PREDICATE(cmp,auxcmp)                            \
  bool operator cmp(const StringPiece& other){                              \
    int ret = memcmp(str_,other.str_,len_ < other.len_ ? len_ : other.len_);\
    return (ret auxcmp 0 || ((ret == 0) && (len_ cmp other.len_)));         \
  }

  STRINGPIECE_BINARY_PREDICATE(<,<);
  STRINGPIECE_BINARY_PREDICATE(<=,<);
  STRINGPIECE_BINARY_PREDICATE(>,>);
  STRINGPIECE_BINARY_PREDICATE(>=,>);

  int compare(const StringPiece& other)
  {
    int ret = memcmp(str_,other.str_,len_ < other.len_ ? len_ : other.len_);
    if(ret == 0)
    {
        if(len_ < other.len_) ret -= 1;
        else ret += 1;
    }
    return ret;
  }

 string as_string() const
 {
    return string(str_,len_);
 }

 void copyToString(string* str)
 {
    str->append(str_,len_);
 }

#ifndef XIN_STD_STRING
 void copyToString(std::string* str)
 {
    str->append(str_,len_);
 }
#endif

 bool startsWith(const StringPiece& x) const
 {
    return (len_ >= x.len_ && memcmp(str_,x.str_,x.len_) == 0);
 }

 private:
  const char* str_;
  int len_;
};

}//namespace xin;

#ifdef HAVE_TYPE_TRAITS
template<> struct __type_traits<xin::StringPiece>{
    typedef __true_type has_trival_default_constructor;
    typedef __true_type has_trival_copy_constructor;
    typedef __true_type has_trival_assignment_constructor;
    typedef __true_type has_trival_destructor;
    typedef __true_type is_POD_type;
};
#endif

std::ostream& operator<<(std::ostream& os,const xin::StringPiece& str)
{
    os << str.data();
    return os;
}
#endif//!XINMJ_BASE_STRINGPIECE_H