#ifndef _FIFO_H
#define _FIFO_H

#define min(x, y) ((x) < (y) ? (x) : (y))


typedef    unsigned char   fifo_u8;

typedef    unsigned int   fifo_u16;


typedef struct ft_fifo {
	fifo_u8 *buffer;	            /* the buffer holding the data */
	fifo_u16 size;	                /* the size of the allocated buffer */
	fifo_u16 in;	                /* data is added at offset (in % size) */
	fifo_u16 out;	                /* data is extracted from off. (out % size) */
    fifo_u16 cnt;                  /* valid data num */
}FT_FIFO;




#if 1
#define  FT_FIFO_PUT(fifo, buff, len) \
do{\
    unsigned char l,Len;\
    unsigned int  temp;\
    unsigned char freesize;\
    if (fifo->cnt < fifo->size)\
    {\
        freesize = fifo->size - fifo->cnt;\
        Len = min(len, freesize);\
        l = min(Len, fifo->size - (fifo->in % fifo->size));\
        local_memcpy((fifo->buffer + (fifo->in % fifo->size)), buff, l);\
        local_memcpy((fifo->buffer), (buff + l), Len - l);\
        temp = fifo->in;\
        temp = temp + Len;\
        temp = temp % fifo->size;\
        fifo->in = temp;\
        fifo->cnt += Len;\
    } \
}while(0)

#endif

extern FT_FIFO *ft_fifo_init(FT_FIFO *fifo, fifo_u8 *buff, fifo_u16 size);
extern FT_FIFO *ft_fifo_alloc(fifo_u16 size);
extern void ft_fifo_free(FT_FIFO *fifo);
extern void ft_fifo_clear(FT_FIFO *fifo);
extern fifo_u16 ft_fifo_put_ext(FT_FIFO *fifo,
				  fifo_u8 *buffer);
extern fifo_u16 ft_fifo_put(FT_FIFO *fifo,
				  fifo_u8 *buffer, fifo_u16 len);
extern fifo_u16 ft_fifo_get(FT_FIFO *fifo,
			   fifo_u8 *buffer, fifo_u16 offset, fifo_u16 len);
// extern fifo_u16 ft_fifo_seek(FT_FIFO *fifo,
// 			   fifo_u8 *buffer, fifo_u16 offset, fifo_u16 len);
extern fifo_u16 ft_fifo_setoffset(FT_FIFO *fifo,fifo_u16 offset);
extern fifo_u16 ft_fifo_getlenth(FT_FIFO *fifo);
extern fifo_u16 ft_fifo_seek(FT_FIFO *fifo, fifo_u8 **buffer,  fifo_u16 len);

#if 0

static void ft_fifo_clear(FT_FIFO *fifo)
{
	//pthread_mutex_lock(&fifo->lock);

    _ft_fifo_clear(fifo);

    //pthread_mutex_unlock(&fifo->lock);
}


static unsigned char ft_fifo_put(FT_FIFO *fifo,
				       unsigned char *buffer, unsigned char len)
{
	unsigned char ret;

	//pthread_mutex_lock(&fifo->lock);
	
	ret = _ft_fifo_put(fifo, buffer, len);
	
	//pthread_mutex_unlock(&fifo->lock);

	return ret;
}


static unsigned char ft_fifo_get(FT_FIFO *fifo,
			   unsigned char *buffer, unsigned char offset, unsigned char len)
{
	unsigned char ret;

	//pthread_mutex_lock(&fifo->lock);

	ret = _ft_fifo_get(fifo, buffer, offset,len);
	//if (fifo->in == fifo->out)
		//fifo->in = fifo->out = 0;

	//pthread_mutex_unlock(&fifo->lock);

	return ret;
}

static unsigned char ft_fifo_getlenth(FT_FIFO *fifo)
{
	unsigned char ret;

	//printf(" ft_fifo_getlenth 1 ");
	//pthread_mutex_lock(&fifo->lock);
	//printf(" ft_fifo_getlenth 2 ");

	ret = _ft_fifo_getlenth(fifo);
	//printf(" ft_fifo_getlenth 3 ");

	//pthread_mutex_unlock(&fifo->lock);
	//printf(" ft_fifo_getlenth 4 ");

	return ret;	
}

static unsigned char ft_fifo_seek(FT_FIFO *fifo,
			   unsigned char *buffer, unsigned char offset, unsigned char len)

{
	unsigned char ret;

	//pthread_mutex_lock(&fifo->lock);

	ret = _ft_fifo_seek(fifo, buffer, offset, len);

	//pthread_mutex_unlock(&fifo->lock);

	return ret;
}

static unsigned char ft_fifo_setoffset(FT_FIFO *fifo,unsigned char offset)

{
	unsigned char ret;

	//pthread_mutex_lock(&fifo->lock);

	ret = _ft_fifo_setoffset(fifo, offset);

	//pthread_mutex_unlock(&fifo->lock);

	return ret;
}
#endif

#endif

