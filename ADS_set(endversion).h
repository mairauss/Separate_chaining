#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

template <typename Key, size_t N = 7>
class ADS_set {
public:
  class Iterator;
  using value_type = Key;
  using key_type = Key;
  using reference = key_type&;
  using const_reference = const key_type&;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = Iterator;
  using const_iterator = Iterator;
  using key_equal = std::equal_to<key_type>; // Hashing
  using hasher = std::hash<key_type>;        // Hashing
private:  
size_type sz {0}, max_sz {0};
  float max_lf {0.7};
key_type co; bool wert=false;

/*Meine Tabelle und Pointers, wo ich die Werte speichere*/
  struct Bucket {
    Key key;
    Bucket* next{nullptr};
    Bucket* head{nullptr};
 
   /*Loescht alle pointers*/
   ~Bucket(){
      delete next; delete head; 
    }
  };

/*table: fuer index*/

  
Bucket* table=nullptr; 

/*hashwert von einem wert berechnet*/
  size_type hash_idx(const key_type& k) const { return hasher{}(k) % max_sz; }
  
/*die neue Werte einfuegen*/
  void insert_unchecked(const key_type& k) {
  size_type idx {hash_idx(k)};
	Bucket * help;
	help = new Bucket;
	help->key =k;
    if(table[idx].head!=nullptr)
    {
      help->next = table[idx].head;
    }
	table[idx].head = help;
    ++sz;
  if(sz>max_sz*10){rehash(max_sz*2);}

 help=nullptr; 
 delete help;
 }

/*sucht den benoetigten wert und retourniert position*/
  Bucket* find_pos(const key_type& k) const {
    size_type idx {hash_idx(k)}; 
    Bucket* b{nullptr};
    if(table[idx].head!=nullptr)
    {
        b=table[idx].head;
    while(b!=nullptr)
    {
      if(key_equal{}(b->key, k)) return b;
      b=b->next;
    }
    } 
  
  delete b;
  
  return nullptr;  
  }

/*fuer Iterator - count position
size_t count_find(const key_type& k) const
{
Bucket *b; size_t n{0};
for(size_t i{0}; i<max_sz; i++)
{   b=table[i].head;
    while(b!=nullptr)
    { ++n;
      if(key_equal{}(b->key, k)) return n;
      b=b->next;
    }
  }
}*/

/*Erstellt neue Tabelle, und kopiert alle alte Werte*/
  void rehash(size_t n) {
    Bucket *old_table {table};
    size_type old_max_sz {max_sz};
    max_sz = n;
    table = new Bucket[max_sz];
    sz = 0; Bucket* b{nullptr};

    for (size_type i{0}; i < old_max_sz; ++i){
    b=old_table[i].head;
    while(b!=nullptr){
	    insert_unchecked(b->key);
	    b=b->next;
	    }      
    }

 delete b;
 delete[] old_table;
}

public:
  ADS_set() 
{ 
  if(max_sz==0)
  {
    table=new Bucket[N]();
    max_sz=N;
  }
}

  ADS_set(std::initializer_list<key_type> ilist): ADS_set{} {
    insert(ilist);
  }

  template<typename InputIt> ADS_set(InputIt first, InputIt last): ADS_set{} {
    insert(first, last);
  }

 ADS_set(const ADS_set& other) {
     if(max_sz==0){
    	table=new Bucket[N]();
    	max_sz=N;
     }
     for (const auto& k: other) insert_unchecked(k);
  }

  ~ADS_set() { delete [] table;}

   ADS_set& operator=(const ADS_set& other) {
    if (this == &other) return *this;
    clear();
    for (const auto& k: other) insert_unchecked(k);
    return *this;
  }

  ADS_set& operator=(std::initializer_list<key_type> ilist) {
    clear();
    insert(ilist);
    return *this;
  }

  size_type size() const { return sz; }
  bool empty() const { return !sz; }

  size_type count(const key_type& key) const {
    return !!find_pos(key);
  }
 
iterator find(const key_type& key) const {
    if (Bucket *pos {find_pos(key)}) return const_iterator{*this,pos,(size_t)hash_idx(key)};
    return end();
  }

/*loescht ganze Tabelle*/
  void clear() {
    delete [] table;
    sz = 0; 
    max_sz=N;
    table=new Bucket[N]();
  }

/*Vertauscht zwei Tabellen*/
  void swap(ADS_set& other) {
    using std::swap;
    swap(sz, other.sz);
    swap(max_sz, other.max_sz);
    swap(max_lf, other.max_lf);
    swap(table, other.table);
  }

  void insert(std::initializer_list<key_type> ilist) {
    for (const auto& k: ilist){ 
      if (!find_pos(k)) insert_unchecked(k);}
  }

 std::pair<iterator,bool> insert(const key_type& key) {
    if (Bucket* pos {find_pos(key)}) return std::make_pair(const_iterator{*this,pos,(size_t)hash_idx(key)}, false);
    insert_unchecked(key);
    return std::make_pair(const_iterator{*this,find_pos(key), (size_t)hash_idx(key)}, true);
  }

  template<typename InputIt> void insert(InputIt first, InputIt last) {
    for (auto it = first; it != last; ++it) {
      if (!find_pos(*it)) {
        insert_unchecked(*it);
      }
    }
  }

/*ein bestimmtes element aus Tabelle loeschen*/
size_type erase(const key_type& key) { 
 size_type idx {hash_idx(key)}; 
    Bucket* pred{nullptr};
    Bucket* act{nullptr};
    
 if(find_pos(key)){
   pred=table[idx].head;
      if(key_equal{}(pred->key, key))
          {
            
            act=table[idx].head;
            table[idx].head=table[idx].head->next;
            --sz;
            act->next=nullptr;
	    delete act;
            return 1; 
          } 
       else {
         act=pred->next;
            while(act != nullptr)
            {
             if(key_equal{}(act->key, key)) 
             {
               
                pred->next = act->next;
		act->next=nullptr;
                pred=nullptr;
		delete act; delete pred;
                --sz;
                return 1;
             }
             else
             {
                pred = act;
                act = act->next;
             }
            }
          }
   } else {wert=true; co=key;}
   
      
    delete pred;
    delete act;
  
    return 0;
}
/*
key_type y() const
{
 if(wert==true)
 {
  Bucket* po=table[first()].head;
  Bucket* b{nullptr};
  bool wert2=false;
  
  for(size_t i{0}; i<max_sz; i++)
  {
  	b=table[i].head;
      while(b!=nullptr)
      {
        if(std::less<key_type>{}(po->key,b->key))
        {
          po=b;
        }
        b=b->next;
      }   
  }
  
  for(size_t i{0}; i<max_sz; i++)
    {
      b=table[i].head;
        while(b!=nullptr)
        {
          if(std::less<key_type>{}(co,b->key) && (std::less<key_type>{}(b->key,po->key) || std::equal_to<key_type>{}(b->key,po->key)))
          {
           wert2=true; po=b;
          }
          
          b=b->next;
        }   
    }

if(wert2==true){ return po->key;}

else throw std::runtime_error ("!Error!");
 }
 
 else throw std::runtime_error ("!Error!");
}*/


key_type y() const
{
  if(wert==true)
  {
    Bucket* b=nullptr;
    Bucket* p=nullptr;
    bool wert1=false;
    
    for(size_t i{0}; i<max_sz; i++)
   {
     b=table[i].head;
    while(b!=nullptr)
    {
      if(p==nullptr || std::less<key_type>{}(p->key,b->key))
      {
        p=b;
      }
      b=b->next;
    }
   }
    
    for(size_t i{0}; i<max_sz; i++)
   {
 	  b=table[i].head;
    while(b!=nullptr)
    {
      if(std::less<key_type>{}(co,b->key)){
      if(std::less<key_type>{}(b->key,p->key) || p==b)
      {
        p=b; wert1=true;
      }
      }
      b=b->next;
    }
   }
   
   if(wert1==true)return p->key;
   else throw std::runtime_error("Error");
  }else throw std::runtime_error("Error");
  
}


/*position von erste element in tabelle*/
size_t first() const{
  int firstElem=0;
  for(int i=0; i<max_sz; i++)
  {
    if(table[i].head!=nullptr)
    {
    firstElem=i;
    return firstElem;
    }
  }
  return firstElem;
}

/*position von letzte element in tabelle
int last() const{
  int lastElem=0;
  for(int i=0; i<max_sz; i++)
  {
    if(table[i].head!=nullptr)
    {
    lastElem=i;
    }
  }
  return lastElem;
}*/

/*letzte element in tabelle
Bucket* endElem(int last) const
{
 Bucket* b=table[last].head;
while(b->next!=nullptr)
  {
     b=b->next;
  }
  return b;
}

//erste element in tabelle
Bucket* firstElem() const
{
  size_t first=0;
  for(size_t i=0; i<max_sz; i++){
    if(table[i].head==nullptr)
    {
  first=i;
  break;
    }
  }
  Bucket* b;
  if(table[first].head!=nullptr){
  b=table[first].head;
  }
  return b;
}*/

 const_iterator begin() const 
{ 
if(sz==0){return end();}
else return const_iterator{*this,table[first()].head, first()};
}

 const_iterator end() const {  return const_iterator{*this, nullptr }; }

/*Ausgabe*/
void dump(std::ostream& o = std::cerr) const {
Bucket* b{nullptr};

o<<"-------Separate Chaining-------"<<std::endl;
    for(size_t i{0}; i<max_sz; i++)
{
	o<<i;
	b=table[i].head;
    while(b!=nullptr)
    {
      o<<"->" << b->key;
      b=b->next;
    }
    if(b==nullptr)
    {
      o<<"..."<<std::endl;
    }
  }

  delete b;
}

friend bool operator==(const ADS_set& lhs, const ADS_set& rhs) {
    if (lhs.sz != rhs.sz) return false;
    for (const auto& k: lhs) 
      if (!rhs.find_pos(k)) return false;
    return true;
  }
  friend bool operator!=(const ADS_set& lhs, const ADS_set& rhs) { return !(lhs==rhs); }
};


template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator {
  
  const ADS_set<Key,N>& ads;
  Bucket* pos;
  size_t first, max_sz;

public:
  using value_type = Key;
  using difference_type = std::ptrdiff_t;
  using reference = const value_type&;
  using pointer = const value_type*;
  using iterator_category = std::forward_iterator_tag;

  explicit Iterator(const ADS_set<Key,N>& ads=nullptr, Bucket * pos=nullptr, size_t first=0): ads(ads), pos{pos}, first{first}
  {  max_sz=ads.max_sz; }

  reference operator*() const { return pos->key; }
  pointer operator->() const { return &pos->key; }

 Iterator& operator++() {
 if (pos->next!=nullptr) {
        pos=pos->next;
	return *this;
    }else{
      first++;
      while(ads.table[first].head==nullptr && first<max_sz )
     {
      ++first;
     }
     if(first>=max_sz) {
     pos=nullptr;
     return *this;
     } else{ pos=ads.table[first].head; }
  }
   return *this;
  }

~Iterator(){pos=nullptr; delete pos;}

  Iterator operator++(int) {
    auto rc = *this;
    ++*this;
    return rc;
  }

  friend bool operator==(const Iterator& lhs, const Iterator& rhs) { return lhs.pos == rhs.pos; }
  friend bool operator!=(const Iterator& lhs, const Iterator& rhs) { return lhs.pos != rhs.pos; }

};

template <typename Key, size_t N> void swap(ADS_set<Key,N>& lhs, ADS_set<Key,N>& rhs) { lhs.swap(rhs); }
#endif // ADS_SET_H
