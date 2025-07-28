/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
 *
 *  I am making my contributions/submissions to this project solely in
 *  my personal capacity and am not conveying any rights to any
 *  intellectual property of any third parties.
 *                                                                          
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *                                                                          
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                          
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/
 */
#pragma once

#include <newalloc.h>
#include <malloc.h>
#include <exception.h>
#include <vector.h>
#include <set.h>
#include <option.h>

template <typename T>
class MemPool {
	private:
		const uint32_t MAX_ELEMENTS;
		const uint32_t CHUNK_SIZE;
		const uint32_t MAX_CHUNKS;
		
		upan::vector<uintptr_t> _freePool;
    upan::set<uintptr_t> _allocatedPool;
		uint32_t _allocatedChunkCount;
    upan::vector<uintptr_t> _allocatedChunks;

	private:
		MemPool(uint32_t maxElements, uint32_t chunkSize) : MAX_ELEMENTS(maxElements), CHUNK_SIZE(chunkSize), MAX_CHUNKS(MAX_ELEMENTS / CHUNK_SIZE), _freePool(maxElements) {
      _allocatedChunkCount = 0 ;
		}

    bool allocateChunk() {
			if(_allocatedChunkCount == MAX_CHUNKS) {
        return false;
      }

			auto address = (uintptr_t)malloc(CHUNK_SIZE * sizeof(T));
      _allocatedChunks.push_back(address);

			for(uint32_t i = 0; i < CHUNK_SIZE; i++) {
				_freePool.push_back(address + i * sizeof(T));
			}

			_allocatedChunkCount++;
			return true;
		}

	public:
		// Factory
		static MemPool<T>& createMemPool(uint32_t size, uint32_t chunkSize) {
			if((size % chunkSize) != 0)
        throw upan::exception(XLOC, "MemPool creation failed. MemPool size: %u is not a multiple of chunk size: %u", size, chunkSize);
			return *new MemPool<T>(size, chunkSize) ;
		}

		~MemPool() {
			for(auto address : _allocatedChunks) {
        free((void*) address);
      }
		}

		upan::option<T&> allocate() {
      //if pool is empty then populate pool
      if (_freePool.empty()) {
        allocateChunk();
      }

      if (_freePool.empty()) {
        return upan::option<T&>::empty();
      }

      auto allocatedAddress = _freePool[_freePool.size() - 1];
      T* object = new((void*)allocatedAddress)T();

      _freePool.pop_back();
      _allocatedPool.insert(allocatedAddress);

      return upan::option<T&>(*object);
		}

		void release(T& object) {
      auto address = (uintptr_t)&object;

      if (!_allocatedPool.erase(address)) {
        throw upan::exception(XLOC, "MemPool release failed. address: 0x%llx is not allocated", address);
      }

      _freePool.push_back(address);
		}
};
