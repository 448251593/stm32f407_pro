#include "fifobuffer.h"
#include "string.h"


/*
***********************************************************
* �������ܣ���ʼ��FIFO�ṹ��
* ��ڲ�����ָ�룬��������С
* ������ֵ��FIFOָ��
* ���ú�����
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
* �������ܣ��ͷ�FIFO
* ��ڲ�����FIFOָ��
* ������ֵ��
* ���ú�����
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
* �������ܣ���ʼ��FIFO�ṹ��
* ��ڲ�����ָ�룬��������С
* ������ֵ��FIFOָ��
* ���ú�����
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
    //���FIFO�Ƿ���ʣ��ռ�
    if(fifo->cnt >= fifo->size)
    {
        return 0;
    }

    //����FIFO��ʣ��ռ�
    // freesize = fifo->size - fifo->cnt;

    //ȡʣ��ռ���д�볤�ȵĽ�С�ߣ���ֹ���
    len = min(len, fifo->size - fifo->cnt);

    //��������
    l = min(len, fifo->size - (fifo->in ) );
    //������ǰλ����ĩβ��һ��
    memcpy((fifo->buffer + (fifo->in )), buffer, l);
    //����ʣ���һ��
    memcpy((fifo->buffer), (buffer + l), len - l);

    //����д���ݵ�ָ��
    // temp = fifo->in;
    temp = fifo->in + len;
    // temp = temp % fifo->size;
    fifo->in = temp % fifo->size;
    fifo->cnt += len;

    return len;

}
/*
***********************************************************
* �������ܣ���Դ�����������ݿ�����FIFO,�ı�д��ַ
* ��ڲ�����FIFOָ�룬Դ������ָ�룬���ݳ���
* ������ֵ��ʵ��д������ݸ���
* ���ú�����
***********************************************************
*/
fifo_u16 ft_fifo_put(FT_FIFO *fifo, fifo_u8  *buffer, fifo_u16 len)
// fifo_u16 ft_fifo_put(FT_FIFO *fifo, fifo_u8  *buffer)
{
    fifo_u16 l = 0;
    // fifo_u16 freesize = 0;
    fifo_u16  temp = 0;
    //���FIFO�Ƿ���ʣ��ռ�
    if(fifo->cnt >= fifo->size)
    {
        return 0;
    }

    //����FIFO��ʣ��ռ�
    // freesize = fifo->size - fifo->cnt;

    //ȡʣ��ռ���д�볤�ȵĽ�С�ߣ���ֹ���
    len = min(len, fifo->size - fifo->cnt);

    //��������
    l = min(len, fifo->size - (fifo->in ) );
    //������ǰλ����ĩβ��һ��
    memcpy((fifo->buffer + (fifo->in )), buffer, l);
    //����ʣ���һ��
    memcpy((fifo->buffer), (buffer + l), len - l);

    //����д���ݵ�ָ��
    // temp = fifo->in;
    temp = fifo->in + len;
    // temp = temp % fifo->size;
    fifo->in = temp % fifo->size;
    fifo->cnt += len;

    return len;

#if 0
    unsigned char l,temp;
    unsigned char freesize;

    //���FIFO�Ƿ���ʣ��ռ�
    if(fifo->out == ((fifo->in + 1) % fifo->size))
        return 0;

    //����FIFO��ʣ��ռ�
    if(fifo->in >= fifo->out)
        freesize = fifo->size - fifo->in + fifo->out - 1;
    else
        freesize = fifo->out - fifo->in - 1;

    //ȡʣ��ռ���д�볤�ȵĽ�С�ߣ���ֹ���
    len = min(len, freesize);

    //��������
    l = min(len, fifo->size - fifo->in % fifo->size );
    //������ǰλ����ĩβ��һ��
    memcpy(fifo->buffer + (fifo->in % fifo->size), buffer, l);
    //����ʣ���һ��
    memcpy(fifo->buffer, buffer + l, len - l);

    //����д���ݵ�ָ��
    temp = fifo->in;
    temp = temp + len;
    temp = temp % fifo->size;
    fifo->in = temp;

    return len;
#endif
}
/*
***********************************************************
* �������ܣ���FIFO�н����ݶ����û�������,�ı����ַ
* ��ڲ�����FIFOָ�룬Դ������ָ�룬ƫ�Ƶ�ַ(����ڶ���ַ),��ȡ���ݳ���
* ������ֵ��ʵ�ʶ�ȡ�ĳ���
* ���ú�����
***********************************************************
*/
fifo_u16 ft_fifo_get(FT_FIFO *fifo, fifo_u8 *buffer, fifo_u16 offset, fifo_u16 len)
{
    fifo_u16 l = 0;
    fifo_u16 datasize = 0;
    fifo_u16  temp = 0;

    //��黺�������Ƿ�����Ч����
    if(0 == fifo->cnt)
    {
        return 0;
    }


    //�����µĶ���ַ
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

    //ȡ��Ҫ��ȡ�����������ݳ��ȵĽ�С��
    len = min(len, datasize);

    //�������ݵ��û�����
    l = min(len, fifo->size - (fifo->out % fifo->size));
    memcpy(buffer, (fifo->buffer + (fifo->out % fifo->size)), l);
    memcpy((buffer + l), (fifo->buffer), len - l);

    //����д���ݵ�ָ��
    temp = fifo->out;
    temp = temp + len;
    temp = temp % fifo->size;
    fifo->out = temp;
    fifo->cnt -= len;

    return len;

#if 0
    unsigned char l,temp;
    unsigned char datasize;

    //��黺�������Ƿ�����Ч����
    if( fifo->out  == fifo->in )
        return 0;

    //������Ч���ݵĳ���
    if(fifo->out >= fifo->in)
        datasize = fifo->size - fifo->out + fifo->in;
    else
        datasize = fifo->in - fifo->out;

    //�����µĶ���ַ
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

    //ȡ��Ҫ��ȡ�����������ݳ��ȵĽ�С��
    len = min(len, datasize);

    //�������ݵ��û�����
    l = min(len, fifo->size - (fifo->out % fifo->size));
    memcpy(buffer, fifo->buffer + (fifo->out % fifo->size), l);
    memcpy(buffer + l, fifo->buffer, len - l);

    //����д���ݵ�ָ��
    temp = fifo->out;
    temp = temp + len;
    temp = temp % fifo->size;
    fifo->out = temp;

    return len;
#endif
}
/*
***********************************************************
* �������ܣ���FIFO�н����ݶ����û�������,<���ı����ַ>
* ��ڲ�����FIFOָ�룬Դ������ָ�룬ƫ�Ƶ�ַ(����ڶ���ַ),��ȡ���ݳ���
* ������ֵ��ʵ�ʶ�ȡ�ĳ���
* ���ú�����
***********************************************************
*/
// fifo_u16 ft_fifo_seek(FT_FIFO *fifo, fifo_u8 *buffer, fifo_u16 offset, fifo_u16 len)
fifo_u16 ft_fifo_seek(FT_FIFO *fifo, fifo_u8 **buffer,  fifo_u16 len)
{
    fifo_u16 l = 0;
    // fifo_u16 datasize = 0;
    fifo_u16  temp = 0;

    //��黺�������Ƿ�����Ч����
    if(fifo->cnt == 0)
    {
        return 0;
    }


    //�����µĶ���ַ
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

    //ȡ��Ҫ��ȡ�����������ݳ��ȵĽ�С��
    len = min(len,  fifo->cnt);

    //�������ݵ��û�����
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
//�����µĶ���ַ
    // l = min(offset, fifo->cnt);

    // temp = fifo->out;
    // temp = temp + l;
    // temp = temp % fifo->size;
    // fifo->out = temp;
    // fifo->cnt -= l;

#if 0
    unsigned char l,temp;
    unsigned char datasize;

    //��黺�������Ƿ�����Ч����
    if( fifo->out  == fifo->in )
        return 0;

    //������Ч���ݵĳ���
    if(fifo->out > fifo->in)
        datasize = fifo->size - fifo->out + fifo->in;
    else
        datasize = fifo->in - fifo->out;

    //�����µĶ���ַ
    if(offset > datasize)
        return -1;
    else
    {
        datasize = datasize - offset;
        temp = fifo->out;
        temp = temp + offset;
        temp = temp % fifo->size;
    }

    //ȡ��Ҫ��ȡ�����������ݳ��ȵĽ�С��
    len = min(len, datasize);

    //�������ݵ��û�����
    l = min(len, fifo->size - (temp % fifo->size));
    memcpy(buffer, fifo->buffer + (temp % fifo->size), l);
    memcpy(buffer + l, fifo->buffer, len - l);

    return len;
#endif
}
/*
***********************************************************
* �������ܣ�����FIFO�Ķ���ʼλ��
* ��ڲ�����FIFOָ�룬ƫ�Ƶ�ַ(����ڶ���ַ)
* ������ֵ��ʵ�����õ�ƫ�Ƶ�ַ
* ���ú�����
***********************************************************
*/
fifo_u16 ft_fifo_setoffset(FT_FIFO *fifo, fifo_u16 offset)
{
    fifo_u16 l = 0;
    fifo_u16  temp = 0;

    //��黺�������Ƿ�����Ч����
    if(fifo->cnt == 0)
    {
        return 0;
    };

    //�����µĶ���ַ
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
    //��黺�������Ƿ�����Ч����
    if( fifo->out  == fifo->in )
        return 0;

    //������Ч���ݵĳ���
    if(fifo->out > fifo->in)
        datasize = fifo->size - fifo->out + fifo->in;
    else
        datasize = fifo->in - fifo->out;

    //�����µĶ���ַ
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

    //��黺�������Ƿ�����Ч����
    if( fifo->out  == fifo->in )
        return 0;

    //������Ч���ݵĳ���
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


