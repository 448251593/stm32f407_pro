#include "fifobuffer.h"
#include "string.h"


/*
***********************************************************
* 函数功能：初始化FIFO结构体
* 入口参数：指针，缓冲区大小
* 返回数值：FIFO指针
* 调用函数：
***********************************************************
*/
FT_FIFO *ft_fifo_init(FT_FIFO *fifo, fifo_u8 *buff, fifo_u16 size)
{
    fifo->buffer = buff;
    memset(fifo->buffer, 0, size);
    fifo->size = size;
    fifo->in = fifo->out = 0;
    fifo->cnt = 0;

    return fifo;
}

#if 0
FT_FIFO *ft_fifo_alloc(unsigned char size)
{
    unsigned char *buffer = NULL;
    FT_FIFO *ret = NULL;

    buffer = malloc(size);
    if (!buffer)
        return ret;

    ret = ft_fifo_init(buffer, size);

    if (!ret)
    {
        free(buffer);
    }

    return ret;
}
#endif
/*
***********************************************************
* 函数功能：释放FIFO
* 入口参数：FIFO指针
* 返回数值：
* 调用函数：
***********************************************************
*/

void ft_fifo_free(FT_FIFO *fifo)
{
#if 0
    free(fifo->buffer);
    free(fifo);
#endif
}


/************************************************************
* 函数功能：初始化FIFO结构体
* 入口参数：指针，缓冲区大小
* 返回数值：FIFO指针
* 调用函数：
************************************************************/
void ft_fifo_clear(FT_FIFO *fifo)
{
    fifo->in = fifo->out = 0;
}


fifo_u16 ft_fifo_put_ext(FT_FIFO *fifo, fifo_u8  *buffer)
{
    fifo_u16 l = 0;
    // fifo_u16 freesize = 0;
    fifo_u16  temp = 0;
    fifo_u16  len = 2;
    //检查FIFO是否有剩余空间
    if(fifo->cnt >= fifo->size)
    {
        return 0;
    }

    //计算FIFO的剩余空间
    // freesize = fifo->size - fifo->cnt;

    //取剩余空间与写入长度的较小者，防止溢出
    len = min(len, fifo->size - fifo->cnt);

    //拷贝数据
    l = min(len, fifo->size - (fifo->in ) );
    //拷贝当前位置至末尾的一段
    memcpy((fifo->buffer + (fifo->in )), buffer, l);
    //拷贝剩余的一段
    memcpy((fifo->buffer), (buffer + l), len - l);

    //更新写数据的指针
    // temp = fifo->in;
    temp = fifo->in + len;
    // temp = temp % fifo->size;
    fifo->in = temp % fifo->size;
    fifo->cnt += len;

    return len;

}
/*
***********************************************************
* 函数功能：将源缓冲区的数据拷贝到FIFO,改变写地址
* 入口参数：FIFO指针，源缓冲区指针，数据长度
* 返回数值：实际写入的数据个数
* 调用函数：
***********************************************************
*/
fifo_u16 ft_fifo_put(FT_FIFO *fifo, fifo_u8  *buffer, fifo_u16 len)
// fifo_u16 ft_fifo_put(FT_FIFO *fifo, fifo_u8  *buffer)
{
    fifo_u16 l = 0;
    // fifo_u16 freesize = 0;
    fifo_u16  temp = 0;
    //检查FIFO是否有剩余空间
    if(fifo->cnt >= fifo->size)
    {
        return 0;
    }

    //计算FIFO的剩余空间
    // freesize = fifo->size - fifo->cnt;

    //取剩余空间与写入长度的较小者，防止溢出
    len = min(len, fifo->size - fifo->cnt);

    //拷贝数据
    l = min(len, fifo->size - (fifo->in ) );
    //拷贝当前位置至末尾的一段
    memcpy((fifo->buffer + (fifo->in )), buffer, l);
    //拷贝剩余的一段
    memcpy((fifo->buffer), (buffer + l), len - l);

    //更新写数据的指针
    // temp = fifo->in;
    temp = fifo->in + len;
    // temp = temp % fifo->size;
    fifo->in = temp % fifo->size;
    fifo->cnt += len;

    return len;

#if 0
    unsigned char l,temp;
    unsigned char freesize;

    //检查FIFO是否有剩余空间
    if(fifo->out == ((fifo->in + 1) % fifo->size))
        return 0;

    //计算FIFO的剩余空间
    if(fifo->in >= fifo->out)
        freesize = fifo->size - fifo->in + fifo->out - 1;
    else
        freesize = fifo->out - fifo->in - 1;

    //取剩余空间与写入长度的较小者，防止溢出
    len = min(len, freesize);

    //拷贝数据
    l = min(len, fifo->size - fifo->in % fifo->size );
    //拷贝当前位置至末尾的一段
    memcpy(fifo->buffer + (fifo->in % fifo->size), buffer, l);
    //拷贝剩余的一段
    memcpy(fifo->buffer, buffer + l, len - l);

    //更新写数据的指针
    temp = fifo->in;
    temp = temp + len;
    temp = temp % fifo->size;
    fifo->in = temp;

    return len;
#endif
}
/*
***********************************************************
* 函数功能：从FIFO中将数据读到用户缓冲区,改变读地址
* 入口参数：FIFO指针，源缓冲区指针，偏移地址(相对于读地址),读取数据长度
* 返回数值：实际读取的长度
* 调用函数：
***********************************************************
*/
fifo_u16 ft_fifo_get(FT_FIFO *fifo, fifo_u8 *buffer, fifo_u16 offset, fifo_u16 len)
{
    fifo_u16 l = 0;
    fifo_u16 datasize = 0;
    fifo_u16  temp = 0;

    //检查缓冲区内是否有有效数据
    if(0 == fifo->cnt)
    {
        return 0;
    }


    //计算新的读地址
    if(offset > fifo->cnt)
    {
        return 0;
    }
    else
    {
        datasize = fifo->cnt - offset;
        temp = fifo->out;
        temp = temp + offset;
        temp = temp % fifo->size;
        fifo->out = temp;
        fifo->cnt -= offset;
    }

    //取需要读取的数据与数据长度的较小者
    len = min(len, datasize);

    //拷贝数据到用户缓存
    l = min(len, fifo->size - (fifo->out % fifo->size));
    memcpy(buffer, (fifo->buffer + (fifo->out % fifo->size)), l);
    memcpy((buffer + l), (fifo->buffer), len - l);

    //更新写数据的指针
    temp = fifo->out;
    temp = temp + len;
    temp = temp % fifo->size;
    fifo->out = temp;
    fifo->cnt -= len;

    return len;

#if 0
    unsigned char l,temp;
    unsigned char datasize;

    //检查缓冲区内是否有有效数据
    if( fifo->out  == fifo->in )
        return 0;

    //计算有效数据的长度
    if(fifo->out >= fifo->in)
        datasize = fifo->size - fifo->out + fifo->in;
    else
        datasize = fifo->in - fifo->out;

    //计算新的读地址
    if(offset > datasize)
        return -1;
    else
    {
        datasize = datasize - offset;
        temp = fifo->out;
        temp = temp + offset;
        temp = temp % fifo->size;
        fifo->out = temp;
    }

    //取需要读取的数据与数据长度的较小者
    len = min(len, datasize);

    //拷贝数据到用户缓存
    l = min(len, fifo->size - (fifo->out % fifo->size));
    memcpy(buffer, fifo->buffer + (fifo->out % fifo->size), l);
    memcpy(buffer + l, fifo->buffer, len - l);

    //更新写数据的指针
    temp = fifo->out;
    temp = temp + len;
    temp = temp % fifo->size;
    fifo->out = temp;

    return len;
#endif
}
/*
***********************************************************
* 函数功能：从FIFO中将数据读到用户缓冲区,<不改变读地址>
* 入口参数：FIFO指针，源缓冲区指针，偏移地址(相对于读地址),读取数据长度
* 返回数值：实际读取的长度
* 调用函数：
***********************************************************
*/
// fifo_u16 ft_fifo_seek(FT_FIFO *fifo, fifo_u8 *buffer, fifo_u16 offset, fifo_u16 len)
fifo_u16 ft_fifo_seek(FT_FIFO *fifo, fifo_u8 **buffer,  fifo_u16 len)
{
    fifo_u16 l = 0;
    // fifo_u16 datasize = 0;
    fifo_u16  temp = 0;

    //检查缓冲区内是否有有效数据
    if(fifo->cnt == 0)
    {
        return 0;
    }


    //计算新的读地址
    // if(offset > fifo->cnt)
    // {
    //     return 0;
    // }
    // else
    // {
        // datasize = fifo->cnt ;
        // temp = fifo->out;
        // temp = temp + offset;
        temp = fifo->out % fifo->size;
    // }

    //取需要读取的数据与数据长度的较小者
    len = min(len,  fifo->cnt);

    //拷贝数据到用户缓存
    // l = min(len, fifo->size - (temp ));
    if(len <= (fifo->size - temp))
    {
        // memcpy(buffer, (fifo->buffer + (temp )), len);
        *buffer =fifo->buffer + (temp );

        return len;
    }
    else
    {
        l =  fifo->size - (temp );
        // memcpy(buffer, (fifo->buffer + (temp )), l);
        *buffer =fifo->buffer + (temp );


        return    l;
        // memcpy((buffer + l), (fifo->buffer), len - l);
    }

    // l = min(len, fifo->size - (temp % fifo->size));
    // memcpy(buffer, (fifo->buffer + (temp % fifo->size)), l);
    // memcpy((buffer + l), (fifo->buffer), len - l);
    // return len;
//计算新的读地址
    // l = min(offset, fifo->cnt);

    // temp = fifo->out;
    // temp = temp + l;
    // temp = temp % fifo->size;
    // fifo->out = temp;
    // fifo->cnt -= l;

#if 0
    unsigned char l,temp;
    unsigned char datasize;

    //检查缓冲区内是否有有效数据
    if( fifo->out  == fifo->in )
        return 0;

    //计算有效数据的长度
    if(fifo->out > fifo->in)
        datasize = fifo->size - fifo->out + fifo->in;
    else
        datasize = fifo->in - fifo->out;

    //计算新的读地址
    if(offset > datasize)
        return -1;
    else
    {
        datasize = datasize - offset;
        temp = fifo->out;
        temp = temp + offset;
        temp = temp % fifo->size;
    }

    //取需要读取的数据与数据长度的较小者
    len = min(len, datasize);

    //拷贝数据到用户缓存
    l = min(len, fifo->size - (temp % fifo->size));
    memcpy(buffer, fifo->buffer + (temp % fifo->size), l);
    memcpy(buffer + l, fifo->buffer, len - l);

    return len;
#endif
}
/*
***********************************************************
* 函数功能：设置FIFO的读起始位置
* 入口参数：FIFO指针，偏移地址(相对于读地址)
* 返回数值：实际设置的偏移地址
* 调用函数：
***********************************************************
*/
fifo_u16 ft_fifo_setoffset(FT_FIFO *fifo, fifo_u16 offset)
{
    fifo_u16 l = 0;
    fifo_u16  temp = 0;

    //检查缓冲区内是否有有效数据
    if(fifo->cnt == 0)
    {
        return 0;
    };

    //计算新的读地址
    l = min(offset, fifo->cnt);

    temp = fifo->out;
    temp = temp + l;
    temp = temp % fifo->size;
    fifo->out = temp;
    fifo->cnt -= l;

    return l;
#if 0
    unsigned char datasize;
    unsigned char l,temp;
    //检查缓冲区内是否有有效数据
    if( fifo->out  == fifo->in )
        return 0;

    //计算有效数据的长度
    if(fifo->out > fifo->in)
        datasize = fifo->size - fifo->out + fifo->in;
    else
        datasize = fifo->in - fifo->out;

    //计算新的读地址
    l = min(offset, datasize);

    temp = fifo->out;
    temp = temp + l;
    temp = temp % fifo->size;
    fifo->out = temp;

    return l;
#endif
}

fifo_u16 ft_fifo_getlenth(FT_FIFO *fifo)
{
    return (fifo->cnt);

#if 0
    unsigned int datasize;
    unsigned char test = 0;

    //检查缓冲区内是否有有效数据
    if( fifo->out  == fifo->in )
        return 0;

    //计算有效数据的长度
    if(fifo->out >= fifo->in)
    {
        datasize = fifo->size - fifo->out + fifo->in;
        test = 1;
    }
    else
    {
        datasize = fifo->in - fifo->out;
        test = 2;
    }
    if(datasize > 20)
    {
        while(1)
        {
            ;
        }
    }

    return datasize;
#endif
}


