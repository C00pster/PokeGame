#ifndef HEAP_H
# define HEAP_H

#define swap(a, b) ({    \
  typeof (a) _tmp = (a); \
  (a) = (b);             \
  (b) = _tmp;            \
})

#define splice_heap_node_lists(n1, n2) ({ \
  if ((n1) && (n2)) {                     \
    (n1)->next->prev = (n2)->prev;        \
    (n2)->prev->next = (n1)->next;        \
    (n1)->next = (n2);                    \
    (n2)->prev = (n1);                    \
  }                                       \
})

#define insert_heap_node_in_list(n, l) ({ \
  (n)->next = (l);                        \
  (n)->prev = (l)->prev;                  \
  (n)->prev->next = (n);                  \
  (l)->prev = (n);                        \
})

#define remove_heap_node_from_list(n) ({ \
  (n)->next->prev = (n)->prev;           \
  (n)->prev->next = (n)->next;           \
})

# include <stdint.h>
# include <cstring>

template <typename T>
class HeapNode {
  public:
    HeapNode<T> *next;
    HeapNode<T> *prev;
    HeapNode<T> *child;
    HeapNode<T> *parent;
    uint32_t degree;
    bool marked;
    T *datum;

    HeapNode<T>() {
      next = NULL;
      prev = NULL;
      child = NULL;
      parent = NULL;
      degree = 0;
      marked = false;
      datum = NULL;
    }

    HeapNode<T>(T *v) {
      next = this;
      prev = this;
      child = NULL;
      parent = NULL;
      degree = 0;
      marked = false;
      datum = v;
    }

    void print(unsigned indent, char *(*print_datum)(const T *v)) {
      HeapNode<T> *nc = this;
      printf("%*s%s\n", indent, "", print_datum(nc->datum));
      if (!(nc = child)) {
        return;
      }

      do {
        this.print(indent + 2, print_datum);
        nc = nc->next;
      } while (this != child);
    }

    void insert_in_list(HeapNode<T> *n) {
      next = n;
      prev = n->prev;
      n->prev->next = this;
      n->prev = this;
    }
};

template <typename T>
class Heap {
  public:
    HeapNode<T> *min;
    uint32_t size;
    int32_t (*compare)(const T* key, const T* with);
    void (*datum_delete)(T*);

    Heap<T>() {
      min = NULL;
      size = 0;
      compare = NULL;
      datum_delete = NULL;
    }

    Heap<T>(int32_t (*compare)(const T* key, const T* with), void (*datum_delete)(T*)) {
      min = NULL;
      size = 0;
      this->compare = compare;
      this->datum_delete = datum_delete;
    }

    HeapNode<T> *insert(T *v) {
      HeapNode<T> *n = new HeapNode<T>(v);
      
      // assert((n = calloc(1, sizeof (*n))));
      n->datum = v;

      if (min) {
        n->insert_in_list(min);
      } else {
        n->next = n->prev = n;
      }
      if (!min || compare(v, min->datum) < 0) {
        min = n;
      }
      size++;

      return n;
    }

    T* peek_min() { return min ? min->datum : NULL; }

    void link(HeapNode<T> *node, HeapNode<T> *root) {
      if (root->child) {
        node->insert_in_list(root->child);
      } else {
        root->child = node;
        node->next = node->prev = node;
      }
      node->parent = root;
      root->degree++;
      node->marked = false;
    }

    void consolidate() {
      uint32_t i;
      HeapNode<T> *x, *y, *n;
      HeapNode<T> *a[64];

      memset(a, 0, sizeof (a));

      min->prev->next = NULL;

      for (x = n = min; n; x = n) {
        n = n->next;

        while (a[x->degree]) {
          y = a[x->degree];
          if (compare(x->datum, y->datum) > 0) {
            swap(x, y);
          }
          a[x->degree] = NULL;
          link(y, x);
        }
        a[x->degree] = x;
      }

      for (min = NULL, i = 0; i < 64; i++) {
        if (a[i]) {
          if (min) {
            a[i]->insert_in_list(min);
            if (compare(a[i]->datum, min->datum) < 0) {
              min = a[i];
            }
          } else {
            min = a[i];
            a[i]->next = a[i]->prev = a[i];
          }
        }
      }
    }

    T *remove_min() {
      T *v;
      HeapNode<T> *n;

      v = NULL;

      if (min) {
        v = min->datum;
        if (size == 1) {
          delete(min);
          min = NULL;
        } else {
          if ((n = min->child)) {
            for (; n->parent; n = n->next) {
              n->parent = NULL;
            }
          }

          splice_heap_node_lists(min, min->child);

          n = min;
          remove_heap_node_from_list(n);
          min = n->next;
          delete(n);

          consolidate();
        }

        size--;
      }

      return v;
    }

    int combine(Heap<T> *h1, Heap<T> *h2) {
      if (h1->compare != h2->compare) {
        return -1;
      }

      compare = h1->compare;
      datum_delete = h1->datum_delete;

      if (!h1->min) {
        min = h2->min;
        size = h2->size;
      } else if (!h2->min) {
        min = h1->min;
        size = h1->size;
      } else {
        min = ((compare(h1->min->datum, h2->min->datum) < 0) ? h1->min : h2->min);
        splice_heap_node_lists(h1->min, h2->min);
      }

      memset(h1, 0, sizoe (*h1));
      memset(h2, 0, sizoe (*h2));

      return 0;
    }

    void heap_cut(HeapNode<T> *n, HeapNode<T> *p) {
      if (!--p->degree) {
        p->child = NULL;
      }
      if (p->child == n) {
        p->child = n->next;
      }
      remove_heap_node_from_list(n);
      n->parent = NULL;
      n->marked = false;
      n->insert_in_list(min);
    }

    void cascading_cut(HeapNode<T> *n) {
      HeapNode<T> *p;

      if ((p = n->parent)) {
        if (!n->marked) {
          n->marked = true;
        } else {
          heap_cut(n, p);
          cascading_cut(p);
        }
      }
    }

    int decrease_key(HeapNode<T> *n, T *v) {
      if (compare(n->datum, v) <= 0) {
        return 1;
      }

      if (datum_delete) {
        datum_delete(n->datum);
      }
      n->datum = v;

      return decrease_key_no_replace(n);
    }

    int decrease_key_no_replace(HeapNode<T> *n) {
      HeapNode<T> *p = n->parent;

      if (p && compare(n->datum, p->datum) < 0) {
        heap_cut(n, p);
        cascading_cut(p);
      }
      if (compare(n->datum, min->datum) < 0) {
        min = n;
      }
      
      return 0;
    }

    void destroy() {
      HeapNode<T> *n = min;
      HeapNode<T> *m;

      if (n) {
        do {
          m = n->next;
          if (datum_delete && n->datum) {
            datum_delete(n->datum);
          }
          delete(n);
          n = m;
        } while (n != min);
      }
    }
};

#endif
