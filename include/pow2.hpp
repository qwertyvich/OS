#pragma once

#include <cmath>             
#include <iostream>           
#include <unordered_map>      // для хранения списков свободных блоков
#include <vector>             //для удобного управления списком блоков

class Allocator_pow_of_2 {
 public:
   struct Header {// содержит указатель next на следующий свободный блок
     Header *next;
   };

   std::unordered_map<size_t, Header *> free_blocks_lists; //списки свободных блоков
              //размер блока
                            //указатель на связный список блоков

   std::vector<size_t> powsOf2 = {16, 32, 64, 128, 256, 512, 1024}; // возможные блоки 
  


   void *start_point;  // ук. на пул
   void *data;
   void *end_point;   // ук. на кон. пула

   static size_t align(size_t size) {  //привожу размер к ближайшему допустимому блоку 2^N
     int i = 0;
     while (pow(2, i) < size || pow(2, i) < 16) {
       i++;
     }
     return (size_t)pow(2, i);
   }

   Allocator_pow_of_2(std::vector<size_t> &blocks_amount);
   ~Allocator_pow_of_2();
   void *allocate(size_t bytes_amount);
   void deallocate(void *ptr);
};


Allocator_pow_of_2::Allocator_pow_of_2(std::vector<size_t> &blocks_amount) { //констурктор
  size_t bytes_sum = 0;
  for (size_t i = 0; i < blocks_amount.size(); i++) {
    bytes_sum += blocks_amount[i] * (sizeof(Header *) + powsOf2[i]);
    //   кол-во блоков*(размер хедера + размер самого блока)
  }

  data = malloc(bytes_sum + (sizeof(Header *) * blocks_amount.size())); //выделяю пул
                                // размером bytes_sum+ кол-во блоков * размер структруы
  start_point = data;

  end_point = (void *)((char *)start_point + bytes_sum + (sizeof(Header *) * blocks_amount.size()));
  // адрес конца пула начало + размер всего пула

  void *point_now = start_point; // установил текущий поинтер

  for (size_t i = 0; i < blocks_amount.size(); i++) {
    Header *head = (Header *)point_now;
    free_blocks_lists[powsOf2[i]] = head; //пустой связный список для каждого размера
    
    point_now = (void *)((char *)point_now + sizeof(Header *)); //cдвигаю point_now на размер одного указателя
  
    for (size_t j = 0; j < blocks_amount[i]; j++) {
      head->next = (Header *)point_now; //cвязываем голову списка с следующим блоком

      head = head->next;

      point_now = (void *)((char *)point_now + powsOf2[i] + sizeof(Header *));
      //теперь указывает на позицию для следующего блока данного размера
      
    }
    head->next = nullptr;
    //последний блок null
  }
}

//  функция allocate
void *Allocator_pow_of_2::allocate(size_t bytes_amount) {
  if (bytes_amount == 0) {
    std::cerr << "Size must be bigger than 0\n";
    return nullptr;
  }

  size_t size = align(bytes_amount);//округлил до степени двойки
  Header *head = free_blocks_lists[size]; //адрес списка  блоков типа хед
  if (head->next == nullptr) {              // проверяю существование блоков
    std::cerr << "Zero free element\n";
    return nullptr;
  }
  Header *alloc_elem = head->next; // взял первый блок
  head->next = head->next->next; // заменяю первый блок на второй
  alloc_elem->next = head;

  return (void *)((char *)alloc_elem + sizeof(Header *));// возвращаю указатель на начало данных внутри выделенного блока
  
}

// Реализация функции deallocate
void Allocator_pow_of_2::deallocate(void *ptr) {
  if (ptr < start_point || ptr > end_point) {
    std::cerr << "Uncorrect ptr\n";
    return;
  }

  Header *ptr_header = (Header *)((char *)ptr - sizeof(Header *));

  for (size_t i = 0; i < free_blocks_lists.size(); i++) {
    if (free_blocks_lists[powsOf2[i]] == ptr_header->next) {
      ptr_header->next = free_blocks_lists[powsOf2[i]]->next;
      free_blocks_lists[powsOf2[i]]->next = ptr_header;  
      return;
    }
  }
  std::cerr << "Uncorrect ptr\n";
}

Allocator_pow_of_2::~Allocator_pow_of_2() { free(data); } // деструктор
