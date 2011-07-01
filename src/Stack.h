#ifndef __STACK_H__
#define __STACK_H__

#define DEFINE_STACK(S_DATA_TYPE, S_SIZE_TYPE, S_TYPE_NAME) typedef struct {S_DATA_TYPE *pBuffer; S_SIZE_TYPE Size; S_SIZE_TYPE Count;} S_TYPE_NAME;

#define S_INIT(Stack, _pBuffer, _Size) \
{\
	Stack.Size = _Size;\
	Stack.Count	= 0;\
	if (_Size > 0)\
		Stack.pBuffer = _pBuffer;\
	else\
		Stack.pBuffer = NULL;\
}

#define S_CLEAR(Stack) \
{\
	Stack.Count	= 0;\
}

#define S_IS_EMPTY(Q) (Q.Count <= 0)
#define S_IS_FULL(Q) (Q.Count >= Q.Size)

#define S_GET_SIZE(Stack) Stack.Size
#define S_GET_COUNT(Stack) Stack.Count

#define S_PEEK(Stack) *(Stack.pBuffer + Stack.Count - 1)

#define S_PUSH(Stack, Val) \
{\
	if (Stack.Count < Stack.Size)\
	{\
		*(Stack.pBuffer + Stack.Count) = Val;\
		Stack.Count++;\
	}\
}

#define S_POP(Stack) \
{\
	if (Stack.Count > 0)\
	{\
		Stack.Count--;\
	}\
}

#endif
