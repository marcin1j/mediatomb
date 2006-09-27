/*  base_queue.h - this file is part of MediaTomb.
                                                                                
    Copyright (C) 2005 Gena Batyan <bgeradz@deadlock.dhs.org>,
                       Sergey Bostandzhyan <jin@deadlock.dhs.org>
                                                                                
    MediaTomb is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
                                                                                
    MediaTomb is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
                                                                                
    You should have received a copy of the GNU General Public License
    along with MediaTomb; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __ZMMF_BASE_QUEUE_H__
#define __ZMMF_BASE_QUEUE_H__

#include "zmm/zmm.h"

namespace zmm
{
    /// \brief a simple stack for a base type. NOT thread safe!
    template <typename T>
    class BaseQueue : public Object
    {
    public:
        inline BaseQueue(int initialCapacity, T emptyType) : Object()
        {
            capacity = initialCapacity;
            this->emptyType = emptyType;
            queueEnd = 0;
            queueBegin = 0;
            overlap = false;
            data = (T *)MALLOC(capacity * sizeof(T));
        }
        
        inline ~BaseQueue()
        {
            FREE(this->data);
        }
        
        inline int getCapacity()
        {
            return capacity;
        }
        
        inline int size()
        {
            if (overlap) return capacity;
            if (queueBegin < queueEnd)
                return queueEnd - queueBegin;
            return capacity - queueEnd + queueBegin;
        }
        
        inline bool isEmpty()
        {
            return (! overlap && queueEnd == queueBegin);
        }
        
        void resize(int requiredSize)
        {
            if(requiredSize > capacity)
            {
                int count = size();
                int oldCapacity = capacity;
                capacity = count + (count / 2);
                if(requiredSize > capacity)
                    capacity = requiredSize;
                data = (T *)REALLOC(data, capacity * sizeof(T));
                log_debug("resizing %d -> %d\n", oldCapacity, capacity);
                if ((overlap && (queueEnd != 0)) || queueBegin > queueEnd)
                {
                    int moveAmount = oldCapacity - queueBegin;
                    int newQueueBegin = capacity - moveAmount;
                    memmove(data + newQueueBegin, data + queueBegin, moveAmount * sizeof(T));
                    queueBegin = newQueueBegin;
                }
                else if (queueEnd == 0)
                    queueEnd = oldCapacity;
                overlap = false;
            }
        }
        
        inline void enqueue(T element)
        {
            if (overlap)
                resize(size() + 1);
            data[queueEnd++] = element;
            if (queueEnd == capacity)
                queueEnd = 0;
            if (queueEnd == queueBegin)
                overlap = true;
        }
        
        inline T dequeue()
        {
            if (isEmpty())
                return emptyType;
            T ret = data[queueBegin++];
            overlap = false;
            if (queueBegin == capacity)
                queueBegin = 0;
            return ret;
        }
        
    protected:
        T *data;
        T emptyType;
        int capacity;
        int queueBegin;
        int queueEnd;
        bool overlap;
    };
}

#endif // __ZMMF_BASE_QUEUE_H__
