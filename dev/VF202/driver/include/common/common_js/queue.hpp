/**
 * @file queue.hpp
 * @author 
 * @brief Queue 队列是一种特殊的线性表，遵循先入先出，后入后出的基本原则，一般来说，它只允许在表的前端进行删除操作，而在表的后端进行插入操作
 *              阻塞和非阻塞
 *                      阻塞队列：
 *                          元素入列时，如果元素数量超过队列总数，会进行阻塞等待，待队列中元素数量未超过队列总数时，就会解除阻塞状态，进而可以继续入列
 *                          元素出列时，如果队列为空的情况下，也会进行阻塞等待
 *                      非阻塞队列：
 *                          元素入列时，如果元素数量超过队列总数，会抛出异常
 *                          元素出列时，如果队列为空的情况下，会取出空值
 *              有界和无界
 *                      有界：有界限，长度大小受到限制
 *                      无界：无限大小(超出界限时就会扩容)
 *              单向链表和双向链表：
 *                      单向链表：每个元素中除了元素本身之外，还存储一个指针，指向后一个元素的地址
 *                      双向链表：每个元素中除了元素本身之外，还有两个指针，一个指向前一个元素的地址，另一个指向后一个元素的地址
 *              队列的存储结构
 *                      链式
 *                      数组
 * 
 * @version 0.1
 * @date 2023-06-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "stdbool.h"
#ifndef _DX_QUEUE_HPP_
#define _DX_QUEUE_HPP_ 1

// push pop peek size empty
// C++ STL中的Queue有哪些地方不适用于现在的场景？
// 我们需要的是一个全局Queue。就相当于Class上的类方法，而不是对象方法
// new Queue("id") Queue.get("id") queue.add() queue.poll() queue.size() queue.destroy() queue.isEmpty() queue.isDistroy()

#ifdef __cplusplus
extern "C" {
#endif 

    void msgPush(unsigned long int name, char* msg);
    char* msgFront(unsigned long int name);
    bool msgEmpty(unsigned long int name);
    int msgSize(unsigned long int name);

#ifdef __cplusplus
}
#endif 





#endif