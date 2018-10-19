#include <iostream>
#include <unistd.h>

struct header_t {
  size_t size;
  struct header_t *next;
  unsigned char is_free;
};

struct header_t *get_free_block(size_t size);
void *malloc(size_t size);
void print_brk();
void print_header(struct header_t *header);
void print_block(void *p);
void print_alloc_list();

namespace {
struct header_t *head, *tail;
pthread_mutex_t global_malloc_lock;
}

////////////////////////////////////////////////////////////////////////
// Implementation.

/**
 * Return the first free block of at least the specified size, or nullptr if no
 * such block exists.
 */
struct header_t *get_free_block(size_t size) {
  struct header_t *curr = head;
  while (curr) {
    if (curr->is_free && curr->size >= size)
      return curr;
    curr = curr->next;
  }
  return nullptr;
}

/**
 * Allocate a block of memory of a specific size on the heap.
 */
void *malloc(size_t size) {
  size_t total_size;
  void *block;
  struct header_t *header;

  if (!size)
    return nullptr;

  // Check for free previously-allocated block of the right size.
  pthread_mutex_lock(&global_malloc_lock);
  header = get_free_block(size);
  if (header) {
    header->is_free = 0;
    pthread_mutex_unlock(&global_malloc_lock);
    return static_cast<void*>(header + 1);
  }

  // If none, allocate a new block.
  total_size = sizeof(struct header_t) + size;
  block = sbrk(static_cast<int>(total_size));
  if (block == reinterpret_cast<void*>(-1)) {
    pthread_mutex_unlock(&global_malloc_lock);
    return nullptr;
  }

  header = static_cast<struct header_t*>(block);
  header->size = size;
  header->is_free = 0;
  header->next = nullptr;
  if (!head)
    head = header;
  if (tail)
    tail->next = header;
  tail = header;
  pthread_mutex_unlock(&global_malloc_lock);
  return static_cast<void*>(header + 1);
}

/**
 * Frees the specified block.
 */
void free(void *block) {
  if (!block)
    return;

  pthread_mutex_lock(&global_malloc_lock);

  // Get block header.
  struct header_t *header = static_cast<struct header_t*>(block) - 1;

  // Check if we're the last allocated block before brk.
  void *program_break = sbrk(0);

  if (static_cast<char*>(block) + header->size == program_break) {
    if (head == tail) {
      head = tail = nullptr;
    } else {
      for (struct header_t *p = head; p != nullptr; p = p->next) {
        if (p->next == tail) {
          p->next = nullptr;
          tail = p;
        }
      }
    }
    size_t total_size = sizeof(struct header_t) + header->size;
    void *result = sbrk(-static_cast<int>(total_size));
    std::cout << "Result: " << result << std::endl;
    pthread_mutex_unlock(&global_malloc_lock);
    return;
  }

  header->is_free = 1;
  pthread_mutex_unlock(&global_malloc_lock);
}

/**
 * Compute and report brk.
 */
void print_brk() {
  void *p = sbrk(0);
  if (p == reinterpret_cast<void*>(-1))
    std::cerr << "sbrk() failed" << std::endl;
  std::cout << "brk: " << p << std::endl;
}

void print_header(struct header_t *header) {
  if (!header)
    return;
  std::cout << "hdr:  " << header << std::endl;
  std::cout << "size: " << header->size << std::endl;
  std::cout << "free: " << static_cast<bool>(header->is_free) << std::endl;
}

/**
 * Print the header for a block.
 */
void print_block(void *block) {
  std::cout << "addr: " << block << std::endl;
  if (!block)
    return;
  struct header_t *h = static_cast<struct header_t*>(block) - 1;
  print_header(h);
}

/**
 * Print the alloc list.
 */
void print_alloc_list() {
  std::cout << "== head" << std::endl;
  print_header(head);
  std::cout << "== tail" << std::endl;
  print_header(tail);
}

int main(int argc, char** argv) {
  for (auto i = 0; i < argc; ++i)
    std::cout << "argv[" << i << "]: " << argv[i] << std::endl;

  print_brk();
  print_alloc_list();
  std::cout << std::endl;

  void *p = malloc(20);
  std::cout << "== malloc'ed block" << std::endl;
  print_block(p);
  print_brk();
  print_alloc_list();

  free(p);
  std::cout << "== free'ed block" << std::endl;
  print_brk();
  print_alloc_list();
}
