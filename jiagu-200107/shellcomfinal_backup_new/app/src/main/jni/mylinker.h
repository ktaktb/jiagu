typedef __u32 Elf32_Addr;
typedef __u16 Elf32_Half;
typedef __u32 Elf32_Off;
typedef __s32 Elf32_Sword;
typedef __u32 Elf32_Word;

#define SOINFO_NAME_LEN 128

#include "./include/linked_list.h"
#include "./UniquePtr.h"
#include "./linker_block_allocator.h"

struct android_namespace_t;

typedef struct {
  uint64_t flags;
  void*   reserved_addr;
  size_t  reserved_size;
  int     relro_fd;
  int     library_fd;
  off64_t library_fd_offset;
  struct android_namespace_t* library_namespace;
} android_dlextinfo;
struct soinfo;
struct link_map {
  Elf32_Addr l_addr;
  char* l_name;
  Elf32_Dyn* l_ld;
  struct link_map* l_next;
  struct link_map* l_prev;
};

class SoinfoListAllocator {
 public:
  static LinkedListEntry<soinfo>* alloc();
  static void free(LinkedListEntry<soinfo>* entry);

 private:
  // unconstructable
  DISALLOW_IMPLICIT_CONSTRUCTORS(SoinfoListAllocator);
};



struct soinfo
{
    typedef LinkedList<soinfo, SoinfoListAllocator> soinfo_list_t;
    char name[SOINFO_NAME_LEN];
    const Elf32_Phdr *phdr;
    int phnum;
    unsigned entry;
    unsigned base;
    unsigned size;

    int unused;  // DO NOT USE, maintained for compatibility.

    unsigned *dynamic;

    unsigned unused2; // DO NOT USE, maintained for compatibility
    unsigned unused3; // DO NOT USE, maintained for compatibility

    soinfo *next;
    unsigned flags;

    const char *strtab;
    Elf32_Sym *symtab;

    unsigned nbucket;
    unsigned nchain;
    unsigned *bucket;
    unsigned *chain;

    unsigned *plt_got;

    Elf32_Rel *plt_rel;
    unsigned plt_rel_count;

    Elf32_Rel *rel;
    unsigned rel_count;

    unsigned *preinit_array;
    unsigned preinit_array_count;

    unsigned *init_array;
    unsigned init_array_count;
    unsigned *fini_array;
    unsigned fini_array_count;

    void (*init_func)(void);
    void (*fini_func)(void);

#if defined(ANDROID_ARM_LINKER)
    /* ARM EABI section used for stack unwinding. */
    unsigned *ARM_exidx;
    unsigned ARM_exidx_count;
#elif defined(ANDROID_MIPS_LINKER)
#if 0
     /* not yet */
     unsigned *mips_pltgot
#endif
     unsigned mips_symtabno;
     unsigned mips_local_gotno;
     unsigned mips_gotsym;
#endif /* ANDROID_*_LINKER */

    unsigned refcount;
    struct link_map linkmap;

    int constructors_called;

    /* When you read a virtual address from the ELF file, add this
     * value to get the corresponding address in the process' address space */
    Elf32_Addr load_bias;
    int has_text_relocations;
    void add_secondary_namespace(android_namespace_t* secondary_ns);

};

struct android_namespace_t {
 public:
  android_namespace_t() : name_(nullptr), is_isolated_(false) {}

  const char* get_name() const { return name_; }
  void set_name(const char* name) { name_ = name; }

  bool is_isolated() const { return is_isolated_; }
  void set_isolated(bool isolated) { is_isolated_ = isolated; }

  const std::vector<std::string>& get_ld_library_paths() const {
    return ld_library_paths_;
  }
  void set_ld_library_paths(std::vector<std::string>&& library_paths) {
    ld_library_paths_ = library_paths;
  }

  const std::vector<std::string>& get_default_library_paths() const {
    return default_library_paths_;
  }
  void set_default_library_paths(std::vector<std::string>&& library_paths) {
    default_library_paths_ = library_paths;
  }

  const std::vector<std::string>& get_permitted_paths() const {
    return permitted_paths_;
  }
  void set_permitted_paths(std::vector<std::string>&& permitted_paths) {
    permitted_paths_ = permitted_paths;
  }

  void add_soinfo(soinfo* si) {
    soinfo_list_.push_back(si);
  }

  void add_soinfos(const soinfo::soinfo_list_t& soinfos) {
    for (auto si : soinfos) {
      add_soinfo(si);
      si->add_secondary_namespace(this);
    }
  }

  void remove_soinfo(soinfo* si) {
    soinfo_list_.remove_if([&](soinfo* candidate) {
      return si == candidate;
    });
  }
  const soinfo::soinfo_list_t& soinfo_list() const { return soinfo_list_; }

  // For isolated namespaces - checks if the file is on the search path;
  // always returns true for not isolated namespace.
  bool is_accessible(const std::string& path);

 private:
  const char* name_;
  bool is_isolated_;
  std::vector<std::string> ld_library_paths_;
  std::vector<std::string> default_library_paths_;
  std::vector<std::string> permitted_paths_;
  soinfo::soinfo_list_t soinfo_list_;

};


template<size_t size>
class SizeBasedAllocator {
 public:
  static void* alloc() {
    return allocator_.alloc();
  }

  static void free(void* ptr) {
    allocator_.free(ptr);
  }

 private:
  static LinkerBlockAllocator allocator_;
};

template<typename T>
class TypeBasedAllocator {
 public:
  static T* alloc() {
    return reinterpret_cast<T*>(SizeBasedAllocator<sizeof(T)>::alloc());
  }

  static void free(T* ptr) {
    SizeBasedAllocator<sizeof(T)>::free(ptr);
  }
};

class LoadTask {
 public:
  struct deleter_t {
    void operator()(LoadTask* t) {
      TypeBasedAllocator<LoadTask>::free(t);
    }
  };

  typedef UniquePtr<LoadTask, deleter_t> unique_ptr;

  static deleter_t deleter;

  static LoadTask* create(const char* name, soinfo* needed_by) {
    LoadTask* ptr = TypeBasedAllocator<LoadTask>::alloc();
    return new (ptr) LoadTask(name, needed_by);
  }

  const char* get_name() const {
    return name_;
  }

  soinfo* get_needed_by() const {
    return needed_by_;
  }
 private:
  LoadTask(const char* name, soinfo* needed_by)
    : name_(name), needed_by_(needed_by) {}

  const char* name_;
  soinfo* needed_by_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(LoadTask);
};



template <typename T>
using linked_list_t = LinkedList<T, TypeBasedAllocator<LinkedListEntry<T>>>;

typedef linked_list_t<soinfo> SoinfoLinkedList;
typedef linked_list_t<const char> StringLinkedList;
typedef linked_list_t<LoadTask> LoadTaskList;



