//Kareem Abdelaty
//30075331
//CPSC 457 A5

#include "memsim.h"
#include <cstdio>
#include <list>
#include <set>
#include <unordered_map>
#include <iterator>
#include <iostream>

// feel free to use this class as a starting point
struct Partition{
	int tag;
	int size;
	int64_t addr;
	Partition(long a,long b,int64_t c){
		tag =a;
		size = b;
		addr = c;
	}
};

typedef std::list<Partition>::iterator PartitionRef;

struct scmp {  
	bool operator()( const PartitionRef & c1, const PartitionRef & c2) const 
		{    if( c1-> size == c2-> size)   
				return c1-> addr < c2-> addr; 
			else       
				return c1-> size < c2-> size; 
		} 
};
//for debugging
/*void print_everything(const std::list<Partition> l, const std::set<PartitionRef,scmp> s,
					const std::unordered_map<long,std::vector<PartitionRef>> o, int i ){
	if(i == 0){
		std::cout << "Allocating" << std::endl;
	}else{
		std::cout << "deAllocating" << std::endl;
	}
	std::cout << "list is" << std::endl;
	for(auto a : l){
		std::cout << "tag " << a.tag << " size " << a.size << " address " << a.addr <<std::endl;
	}
	std::cout << std::endl;
	std::cout << "Set is" << std::endl;
	for(auto a : s){
		std::cout << "tag " << a->tag << " size " << a->size << " address " << a->addr <<std::endl;
	}
	std::cout << std::endl;
	std::cout << "unordered_map is " << std::endl;
	for(auto a : o){
		std::cout << "tag is " << a.first << std::endl;  
		for(auto b : a.second){
				std::cout << "tag " << b->tag << " size " << b->size << " address " << b->addr <<std::endl;
		}
	}
	std::cout << std::endl;
	std::cout << std::endl;
	
}*/
struct Simulator {
  int page_size_;
  int max_free;
  int64_t n_pages;
  int64_t next_partition_address;
  std::list<Partition> partitions;
  std::set<PartitionRef,scmp> free_blocks;
  std::unordered_map<long,	std::vector<PartitionRef>> tagged_blocks;
  Simulator(int64_t page_size) { 
	page_size_ = page_size; 
	max_free =0;
	n_pages = 0;
	next_partition_address = 0;
	
	
	}
  void allocate(int tag, int size) { 
//		if request size is zero do nothing
		if(size <= 0){
			return;
		}
		//find if an empty space already exists
	     std::list<Partition> dummy { Partition(-1,size,0) };
		 auto sbesti = free_blocks.lower_bound( dummy.begin());
		 PartitionRef best_free_block_iter = partitions.end();
		 if( sbesti != free_blocks.end()){
			 best_free_block_iter = * sbesti;
		 }
		 //if an empty space wasnt found request memory from os
		 if( best_free_block_iter == partitions.end()){
			if(partitions.size()>1 &&partitions.back().tag == 0){
					int have  = partitions.back().size;
					int want  = size -have;
					if(want % page_size_ == 0){
						int64_t assign = want/page_size_;
						n_pages += assign;
						Partition p = Partition(tag,size,partitions.back().addr);
						free_blocks.erase(std::next(partitions.end(), -1));
						partitions.pop_back();
						partitions.push_back(p);
						tagged_blocks[tag].push_back(std::next(partitions.end(), -1));
						next_partition_address += want;
					}else{
						int64_t assign = (want/page_size_) + 1;
						n_pages += assign;
						Partition p = Partition(tag,size,partitions.back().addr);
						free_blocks.erase(std::next(partitions.end(), -1));
						partitions.pop_back();
						partitions.push_back(p);
						next_partition_address += want;
						tagged_blocks[tag].push_back(std::next(partitions.end(), -1));
						int64_t f = ((assign*page_size_)-want);
						Partition p2 = Partition(0,f,next_partition_address);
						partitions.push_back(p2);
						free_blocks.insert(std::next(partitions.end(), -1));
						next_partition_address += f;
					}

			}else{		
				if(size % page_size_ == 0){
					int64_t assign = size/page_size_;
					n_pages += assign;
					Partition p = Partition(tag,size,next_partition_address);
					partitions.push_back(p);
					tagged_blocks[tag].push_back(std::next(partitions.end(), -1));
					next_partition_address += size;
				}else{
					int64_t assign = (size/page_size_) + 1;
					n_pages += assign;
					Partition p = Partition(tag,size,next_partition_address);
					partitions.push_back(p);
					next_partition_address += size;
					tagged_blocks[tag].push_back(std::next(partitions.end(), -1));
					int64_t f = ((assign*page_size_)-size);
					Partition p2 = Partition(0,f,next_partition_address);
					partitions.push_back(p2);
					free_blocks.insert(std::next(partitions.end(), -1));
					next_partition_address += f;
				}
			}
		//otherwise assign to the free block	
		 }else{
			 if(size == (best_free_block_iter->size)){
				best_free_block_iter->tag  = tag;
				tagged_blocks[tag].push_back(best_free_block_iter);
				free_blocks.erase(sbesti);
			}else{
				int64_t remaining =  best_free_block_iter->size - size;
				Partition p = Partition(tag,size,best_free_block_iter->addr);
				PartitionRef i = partitions.insert(best_free_block_iter,p);
				tagged_blocks[tag].push_back(i);
				Partition p2 = Partition(0,remaining,((best_free_block_iter->addr) + size));
				PartitionRef i2 = partitions.insert(best_free_block_iter,p2);
				partitions.erase(best_free_block_iter);
				free_blocks.erase(sbesti);
				free_blocks.insert(i2);
				
			}
		 }
		 //print_everything(partitions,free_blocks,tagged_blocks,0);

  }
  void deallocate(int tag) {
		for(auto i : tagged_blocks[tag]){
			if(i == partitions.begin()){
				if(partitions.size()==1){
					Partition p = Partition(0,(i->size),0);
					PartitionRef i4 = partitions.insert(i,p);
					free_blocks.insert(i4);
					partitions.erase(i);	
				}else{
					bool empty_after =false;
					PartitionRef i2  = std::next(i,1);
					if(i2->tag == 0){
						empty_after = true;
					}
					if(empty_after){
						Partition p = Partition(0,((i->size)+(i2->size)),0);
						PartitionRef i4 = partitions.insert(i,p);
						auto i5 = free_blocks.find(i2);
						if(i5 !=free_blocks.end()){
							free_blocks.erase(i5);
						}
						free_blocks.insert(i4);
						partitions.erase(i);
						partitions.erase(i2);		
					}else{
						Partition p = Partition(0,(i->size),0);
						PartitionRef i4 = partitions.insert(i,p);
						free_blocks.insert(i4);
						partitions.erase(i);
						
					}
					
				}
				
			}else if(i == std::next(partitions.end(), -1) && partitions.size() > 1){
					bool empty_before = false;
					PartitionRef i3 = std::next(i,-1);
					if(i3->tag == 0){
						empty_before = true;
					}
					if(empty_before){
						Partition p = Partition(0,((i->size)+(i3->size)),i3->addr);
						PartitionRef i4 = partitions.insert(i3,p);
						auto i6 = free_blocks.find(i3);
						if(i6 != free_blocks.end()){
							free_blocks.erase(i6);
						}
						free_blocks.insert(i4);
						partitions.erase(i3);
						partitions.erase(i);
					}else{
						Partition p = Partition(0,(i->size),i->addr);
						PartitionRef i4 = partitions.insert(i,p);
						free_blocks.insert(i4);
						partitions.erase(i);
						
					}
					
								
			}else{
				bool empty_after =false;
				bool empty_before = false;
				PartitionRef i2  = std::next(i,1);
				PartitionRef i3 = std::next(i,-1);
				if(i2->tag == 0){
					empty_after = true;
				}
				if(i3->tag == 0){
					empty_before = true;
				}
				if(empty_after&&empty_before){
					Partition p = Partition(0,((i->size)+(i2->size)+(i3->size)),i3->addr);
					PartitionRef i4 = partitions.insert(i3,p);
					auto i5 = free_blocks.find(i2);
					auto i6 = free_blocks.find(i3);
					if(i5 != free_blocks.end()){
						free_blocks.erase(i5);
					}
					if(i6 != free_blocks.end()){
						free_blocks.erase(i6);
					}
					free_blocks.insert(i4);
					partitions.erase(i3);
					partitions.erase(i);
					partitions.erase(i2);
				}else if(empty_after&&!empty_before){
					Partition p = Partition(0,((i->size)+(i2->size)),i->addr);
					PartitionRef i4 = partitions.insert(i,p);
					auto i5 = free_blocks.find(i2);
					if(i5 != free_blocks.end()){
						free_blocks.erase(i5);
					}
					free_blocks.insert(i4);
					partitions.erase(i);
					partitions.erase(i2);
					
				}else if(empty_before&&!empty_after){
					Partition p = Partition(0,((i->size)+(i3->size)),i3->addr);
					PartitionRef i4 = partitions.insert(i3,p);
					auto i6 = free_blocks.find(i3);
					if(i6 != free_blocks.end()){
						free_blocks.erase(i6);
					}
					free_blocks.insert(i4);
					partitions.erase(i3);
					partitions.erase(i);
				}else{
					Partition p = Partition(0,(i->size),i->addr);
					PartitionRef i4 = partitions.insert(i,p);
					free_blocks.insert(i4);
					partitions.erase(i);
					
				}	
			}
			
		}
		tagged_blocks[tag].clear();
		//print_everything(partitions,free_blocks,tagged_blocks,1);
	  }
  void getStats(MemSimResult & result)
  {
    // return the size of the maximum free partition (set to 0 if no free partitions exist)
	if(!free_blocks.empty()){
		max_free = (*(std::next(free_blocks.end(), -1)))->size;
	}
    result.max_free_partition_size = max_free;
    // return the total number of pages requested
    result.n_pages_requested = n_pages;
  }
};

// re-implement the following function
// ===================================
// input parameters:
//    page_size: integer in range [1..1,000,000]
//    requests: array of requests, each with tag and size
// output parameters:
//    result: populate with correct values before returning
void mem_sim(int64_t page_size, const std::vector<Request> & requests, MemSimResult & result)
{
  Simulator sim(page_size);
  for (const auto & req : requests) {
    if (req.tag < 0) {
      sim.deallocate(-req.tag);
    } else {
      sim.allocate(req.tag, req.size);
    }
  }
  sim.getStats(result);
}
