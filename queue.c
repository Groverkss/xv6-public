#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

void
push (struct Queue *list, struct proc *element)
{
  if (list->size == NPROC) {
    panic("Proccess limit exceeded");
  }  

  list->array[list->tail] = element;
  list->tail++;
  if (list->tail == NPROC + 1) {
    list->tail = 0;
  }
  list->size++;
}

void
pop(struct Queue *list)
{
  if (list->size == 0) {
    panic("Poping from empty queue");
  }

  list->head++;
  if (list->head == NPROC + 1) {
    list->head = 0;
  }

  list->size--;
}

struct proc*
front(struct Queue *list)
{
  if (list->head == list->tail) {
    return 0;
  } 
  return list->array[list->head];
}

void 
qerase(struct Queue *list, int pid) 
{
  for (int curr = list->head; curr != list->tail; curr = (curr + 1) % (NPROC + 1)) {
    if (list->array[curr]->pid == pid) {
      struct proc *temp = list->array[curr];
      list->array[curr] = list->array[(curr + 1) % (NPROC + 1)];
      list->array[(curr + 1) % (NPROC + 1)] = temp;
    } 
  }

  list->tail--;
  if (list->tail < 0) {
    list->tail = NPROC;
  }
}
