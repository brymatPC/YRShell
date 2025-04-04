#ifndef BufferedSerial_h
#define BufferedSerial_h

#include "HardwareSpecific.h"

#ifdef PLATFORM_ARDUINO

#include "CircularQ.h"

/** \brief BufferedSerial - wrapper arouns the Arduino HardwareSerial class to interface to queues

 Wrapper arouns the Arduino HardwareSerial class to interface to queues.

 */
class BufferedSerial : public Sliceable {
protected:
  HardwareSerial *m_hs; /**< Pointer to the HardwareSerial object */
  CircularQBase<char>* m_nextQ; /**< Pointer to the queue which will receive data from the HardwareSerial object */
  CircularQBase<char>* m_previousQ; /**< Pointer to the queue which will supply data to the HardwareSerial object */
  
public:
   virtual const char* sliceName( ) { return "BufferedSerial"; }
/** \brief BufferedSerial - constructor

 Constructor

 */
  BufferedSerial( HardwareSerial* hs);
/** \brief init - sets up the queues

 Sets up the queues. A null value is permissible to indicate there is no queue

 */
  void init(  CircularQBase<char>& nq, CircularQBase<char>& pq);
/** \brief slice - move data from the HardwareSerial object to / from the queues

 Move data from the HardwareSerial object to / from the queues

 */
  void slice( void);
  void begin( uint32_t baud);
  void end( void);
  void setBaud( uint32_t baud);
  
};



extern BufferedSerial BSerial;  
#ifdef ENABLE_SERIAL1
extern BufferedSerial BSerial1;  
#endif
#ifdef ENABLE_SERIAL2
extern BufferedSerial BSerial2;  
#endif
#ifdef ENABLE_SERIAL3
extern BufferedSerial BSerial3;  
#endif


#endif

#ifdef PLATFORM_STM32_VSCODE

#include "CircularQ.h"

#ifndef SERIAL_TX_BUFFER_SIZE
#define SERIAL_TX_BUFFER_SIZE 256
#endif
#ifndef SERIAL_RX_BUFFER_SIZE
#define SERIAL_RX_BUFFER_SIZE 256
#endif
#ifndef SERIAL_TX_CHUNK_SIZE
#define SERIAL_TX_CHUNK_SIZE (SERIAL_TX_BUFFER_SIZE/4)
#endif

/** \brief BufferedSerial - wrapper around the STM32 HAL UART to interface to queues

 Wrapper around the STM32 HAL UART to interface to queues.

 */
class BufferedSerial : public Sliceable {
  protected:
    UART_HandleTypeDef *m_phandler; /**< Pointer to the UART handler object */
    CircularQ<char, SERIAL_RX_BUFFER_SIZE> m_nextQ; /**< Pointer to the queue which will receive data from the Serial object */
    CircularQ<char, SERIAL_TX_BUFFER_SIZE> m_previousQ; /**< Pointer to the queue which will supply data to the Serial object */
    
  public:

    enum class STM32SerialError {
      none = 0,
      nullHandler,
      noDma,
      handlerBusy,
      handlerError,
      handlerTimeout,
      txFailure
    };

     virtual const char* sliceName( ) { return "BufferedSerial"; }
  /** \brief BufferedSerial - constructor
  
   Constructor
  
   */
    BufferedSerial( UART_HandleTypeDef* phandler);
  /** \brief init - sets up the queues
  
   Sets up the queues. A null value is permissible to indicate there is no queue
  
   */
    void init( CircularQBase<char>& nq, CircularQBase<char>& pq);
  /** \brief slice - move data from the HardwareSerial object to / from the queues
  
   Move data from the HardwareSerial object to / from the queues
  
   */
    void slice( void);
    STM32SerialError begin( uint32_t baud);
    void end( void);
    STM32SerialError setBaud( uint32_t baud);

  private:
    bool m_initialized;
    uint16_t m_bytesTx;
    STM32SerialError m_error;

    STM32SerialError initUart();
    void processRX( void);
    void processTX( void);

    static STM32SerialError halStatusToError(HAL_StatusTypeDef status);    
  };

  #ifdef ENABLE_SERIAL1
  extern BufferedSerial BSerial1;  
  #endif
  #ifdef ENABLE_SERIAL2
  extern BufferedSerial BSerial2;  
  #endif
  #ifdef ENABLE_SERIAL3
  extern BufferedSerial BSerial3;  
  #endif

#endif

#endif
