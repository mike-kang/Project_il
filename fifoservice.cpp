#include "fifoservice.h"
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "tools/log.h"
#include "tools/memory.h"
#include <mcheck.h>
#include "maindelegator.h"



#define CMD_FIFO "/tmp/cmd"
#define RES_FIFO "/tmp/response"

#define LOG_TAG "FifoService"

using namespace std;

FifoService::FifoService(MainDelegator* md): m_bRunningMtrace(false), m_md(md)
{
  LOGV("FifoService+++\n");
  umask(0);                           /* So we get the permissions we want */
  
  if (mkfifo(CMD_FIFO, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1 && errno != EEXIST){
    LOGE("mkfifo %s", CMD_FIFO);
    throw 0;
  }
  
  if (mkfifo(RES_FIFO, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1 && errno != EEXIST){
    LOGE("mkfifo %s", RES_FIFO);
    throw 0;
  }



  m_thread = new Thread<FifoService>(&FifoService::run, this, "FifoService");
  printf("FifoService: this %p\n", this);
  LOGV("FifoService---\n");
    
}

FifoService::~FifoService()
{
  if(m_thread)
    delete m_thread;
  close(m_cmdFd);
}
void FifoService::run()
{
  char buf[100];

  m_cmdFd = open(CMD_FIFO, O_RDONLY);
  if (m_cmdFd == -1)
    LOGE("open %s", CMD_FIFO);
  int dummyFd = open(CMD_FIFO, O_WRONLY);
  if (dummyFd == -1)
    LOGE("open %s", CMD_FIFO);

  while(1){
    
    //LOGV("readLine+++\n");
    int n = readLine(m_cmdFd, buf, 100);
    //LOGV("readLine %d---\n", n);
    if(n > 0){
      m_responseFd = open(RES_FIFO, O_WRONLY);
      if (m_responseFd == -1)
        LOGE("open %s", RES_FIFO);

      if(!strncmp("memory", buf, 6)){
        sprintf(buf, "%d\n", tools::memory::getHeapSize());
        LOGV("cmd:memory %s\n", buf);
        write(m_responseFd, buf, strlen(buf) + 1);
      }
      else if(!strncmp("mtrace", buf, 6)){
        if(m_bRunningMtrace){
          write(m_responseFd, "***** stop mtrace *****\n", strlen("***** stop mtrace *****\n"));
          muntrace();
          m_bRunningMtrace = false;
        }
        else{
          write(m_responseFd, "***** start mtrace *****\n", strlen("***** start mtrace *****\n"));
          mtrace();
          m_bRunningMtrace = true;
        }

      }
      else if(!strncmp("sn", buf, 2)){
        buf[n-1] = '\0';
        m_md->onData(&buf[3]);
        write(m_responseFd, "ok", 2);
      }
      close(m_responseFd);
        
    }
  }
}


std::size_t FifoService::readLine(int fd, char *buffer, std::size_t n)
{
    std::size_t numRead;                    /* # of bytes fetched by last read() */
    std::size_t totRead;                     /* Total bytes read so far */
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;                       /* No pointer arithmetic on "void *" */

    totRead = 0;
    for (;;) {
        numRead = read(fd, &ch, 1);

        if (numRead == -1) {
            if (errno == EINTR)         /* Interrupted --> restart read() */
                continue;
            else
                return -1;              /* Some other error */

        } else if (numRead == 0) {      /* EOF */
            if (totRead == 0)           /* No bytes read; return 0 */
                return 0;
            else                        /* Some bytes read; add '\0' */
                break;

        } else {                        /* 'numRead' must be 1 if we get here */
            if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
                totRead++;
                *buf++ = ch;
            }

            if (ch == '\n')
                break;
        }
    }

    *buf = '\0';
    return totRead;
}


