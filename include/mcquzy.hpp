#ifndef MCCUZY_H            
#define MCCUZY_H         

#include <cmath>            

class McCuzyAllocator {// распределение памяти
 public:
   typedef void value_type;             // показал что аллокатор выдаёт безтиповую память
   typedef value_type *pointer;         
   McCuzyAllocator() = default;         
   ~McCuzyAllocator() { ::free(pointer_start); } 
   

   void free() {
    // static_cast - приведение типа к хедер
     auto *header = static_cast<Header *>(pointer_start); // указатель на начало пула памяти помечаю его 
                                                          //как указатель на стр. Header 
     header->size = (size_total - size_header); // размер первого блока равен всему размеру памяти
     header->available = true;                  // пометил как доступеный
     
     size_used = size_header; // сбросил счётчик
   };

   //округляею size  вверх до ближайшей степени двойки
   size_t approximation(size_t size) {
     int i = 0;
     while (pow(2, i) < size) {
       i++;
     }
     return (size_t)pow(2, i);
   };

   pointer allocate(size_t size) { // возвращаю указатель
     if (size <= 0) {
       std::cerr << "size must be bigger than 0\n";
       return nullptr;
     }
     
     size = approximation(size);  // округлил запрошенный размер до ближайшей степени 2
     if (size > size_total - size_used) { // если размер двойки больше, чем памяти то, возвращаем nullptr
       return nullptr;
     }
     //find()
     auto *header = find(size); // ищу блок памяти свободный 
     if (header == nullptr) {
       return nullptr;
     }
    
     block_separation(header, size); //если блок больше нужного раздаеляю

     return header + 1;
     
   };

   void deallocate(pointer ptr) {
     if (!check_address(ptr)) {
       return;
     }
  
     auto *header = static_cast<Header *>(ptr) - 1;     // полуил указатель на заголовок освобождаемого блока

     header->available = true;
     size_used -= header->size; // уменьшил счётчик
     defragmentation(header); //пытаюсь слить этот блок с соседним
     
   };

   explicit McCuzyAllocator(size_t size) {
     size = approximation(size); // округляем запрошенный размер
 
     if ((pointer_start = malloc(size)) == nullptr) { // выделил память
       std::cerr << "failed to allocate memory\n";
       return;
     }
     
     size_total = size; 

     pointer_end = static_cast<void *>(static_cast<char *>(pointer_start) + size_total); 
     // привожу к чару для адрсной арефметики 
     // указывает на первый байт за концом выделенного пула

     auto *header = (Header *)pointer_start;
     header->size = (size_total - size_header);
     header->size_previous = 0; // предыдущего блока нет
     size_used = size_header;
     header->available = true;// началбнйы блок тут свободный
     
   };
   size_t getRealBlockSize(void* userPtr) {
    if (!userPtr) return 0;
    auto hdr = reinterpret_cast<Header*>(userPtr);
    hdr = hdr - 1;
    return hdr->size + sizeof(Header);
};

   

 private:
   struct Header {
    public:
     size_t size;
     size_t size_previous;
     bool available;            //флаг доступности
     inline Header *next() { return (Header *)((char *)(this + 1) + size); } //следующий блок
     inline Header *previous() {                                              //предыдщуий блок
       return (Header *)((char *)this - size_previous) - 1;         
     }
   };
   

   const size_t size_header = sizeof(Header); // просто конст. размера структруы
   pointer pointer_start = nullptr;
   pointer pointer_end = nullptr;
   size_t size_total = 0;
   size_t size_used = 0;

   Header *find(size_t size) {
     auto *header = static_cast<Header *>(pointer_start);

     while (!header->available || header->size < size) { //скипаю недоступные блооки и маленькие блоки
       header = header->next();
       if (header >= pointer_end) {
         return nullptr;
       }
     }
     return header;
     // вернул указатель на ptr
   }

   void block_separation(Header *header, size_t chunk) {
     size_t size_block = header->size;
     header->size = chunk; //отрезал кусок
     header->available = false; //занял блок !
     if (size_block - chunk >= size_header) { // проверка на остаток размера
       auto *next = header->next();  // указатель на некст блок
       next->size_previous = chunk;  // размер предыдущего или только что занятого
       next->size = size_block - chunk - size_header;

       next->available = true;

       size_used += chunk + size_header;
       
       auto *followed = next->next(); //следующий х2 блок
       if (followed < pointer_end) {
         followed->size_previous = next->size; // если ок даю след блоку этот адрес

       }
     } else {
       header->size = size_block;  // если места нехватило то даю в сайз весь блок
       size_used += size_block;
     }
   }

   bool check_address(void *ptr) {
     auto *header = static_cast<Header *>(pointer_start);
     // Начинаем с первого блока (заголовка) в пуле
     while (header < pointer_end) {
       if (header + 1 == ptr) {
         return true;
       }
       header = header->next();
       // Переходим к следующему блоку и повторяем
     }
     return false;
   }
   //объединяю блок с соседним
   void defragmentation(Header *header) {
     if (previous_free(header)) {
       auto *previous = header->previous();
       if (header->next() < pointer_end) {
         header->next()->size_previous += previous->size + size_header;
       }
       previous->size += header->size + size_header;  //увеличил размер предыдущего   
       size_used -= size_header;
       header = previous;
       
     }
     if (next_free(header)) {  //если следующий блок свободен
       header->size += size_header + header->next()->size; // сложил размеры
       size_used -= size_header;
       auto *next = header->next();
       if (next != pointer_end) {
         next->size_previous = header->size;
         //перезаписал size_previous
       }
     }
   }

   //существует ли предыдущий блок и доступен
   bool previous_free(Header *header) {
     auto *previous = header->previous();
     return header != pointer_start && previous->available;
     
   }

   //существует ли следующий блок
   bool next_free(Header *header) {
     auto *next = header->next();
     return header != pointer_end && next->available;
     
   }
};
#endif  
