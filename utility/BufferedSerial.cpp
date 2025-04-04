#include "BufferedSerial.h"
#include "YRShellInterpreter.h"
#ifdef PLATFORM_ARDUINO
BufferedSerial::BufferedSerial( HardwareSerial* hs) {
	m_hs = hs; 
}
void BufferedSerial::init(  CircularQBase<char>& nq, CircularQBase<char>& pq) {
	m_nextQ = &nq;
	m_previousQ = &pq;
}
void BufferedSerial::slice( void) {
	int c;
	if( m_nextQ != NULL) {
		while( m_nextQ->spaceAvailable()) {
			c = m_hs->read();
			if( c == -1) {
				break;
			}
			m_nextQ->put( c);
		}
	}	
	if( m_previousQ != NULL) {
		while( m_previousQ->valueAvailable() && m_hs->availableForWrite() > 0) {
			m_hs->write( m_previousQ->get());
		}
	}
}
void BufferedSerial::begin( uint32_t baud) {
	m_hs->begin( baud);
}
void BufferedSerial::end( void) {
	m_hs->end( );
}
void BufferedSerial::setBaud( uint32_t baud) {
	m_hs->end();
	m_hs->begin( baud);
}

BufferedSerial BSerial( &Serial);  
#ifdef ENABLE_SERIAL1
BufferedSerial BSerial1( &Serial1);  
#endif
#ifdef ENABLE_SERIAL2
BufferedSerial BSerial2( &Serial2);  
#endif
#ifdef ENABLE_SERIAL3
BufferedSerial BSerial3( &Serial3);  
#endif

#endif

#ifdef PLATFORM_STM32_VSCODE
BufferedSerial::BufferedSerial( UART_HandleTypeDef* phandler) :
	m_phandler(phandler),
	m_nextQ(),
	m_previousQ(),
	m_initialized(false),
	m_bytesTx(0),
	m_error(STM32SerialError::none)
{
}
BufferedSerial::STM32SerialError BufferedSerial::initUart() {
	if(m_phandler == NULL) return STM32SerialError::nullHandler;
	// Only DMA is currently supported
	if(!m_phandler->hdmarx) return STM32SerialError::noDma;
	if(!m_phandler->hdmatx) return STM32SerialError::noDma;

	HAL_StatusTypeDef handlerStatus = HAL_UART_Init(m_phandler);
	if(handlerStatus == HAL_OK) {
		handlerStatus = HAL_UART_Receive_DMA( m_phandler, (unsigned char*)m_nextQ.getBuffer(), m_nextQ.size());
	}
	return halStatusToError(handlerStatus);
}
void BufferedSerial::init(  CircularQBase<char>& nq, CircularQBase<char>& pq) {
	m_nextQ.setNextQ( nq);
	m_previousQ.setPreviousQ( pq);
}
void BufferedSerial::slice( void) {
	processRX();
	processTX();
}
BufferedSerial::STM32SerialError BufferedSerial::begin( uint32_t baud) {
	if(m_phandler == NULL) return STM32SerialError::nullHandler;
	if(m_initialized) {
		end();
	}
	m_phandler->Init.BaudRate = baud;
	BufferedSerial::STM32SerialError result = initUart();
	if(result == STM32SerialError::none) {
		m_initialized = true;
	}
	return result;
}
void BufferedSerial::end( void) {
	if( m_initialized) {
		HAL_UART_DeInit(m_phandler);
		m_initialized = false;
	}
}
BufferedSerial::STM32SerialError BufferedSerial::setBaud( uint32_t baud) {
	end();
	return begin(baud);
}
void BufferedSerial::processRX(void) {
	if(m_phandler == NULL) return;
	uint16_t used;
	uint16_t prevUsed    = m_nextQ.used();
	uint16_t availableQ  = m_nextQ.size();
	uint16_t bytesInDMA  = __HAL_DMA_GET_COUNTER( m_phandler->hdmarx);
	m_nextQ.setHead(availableQ - bytesInDMA);
	used = m_nextQ.used();
	// Catch overflow condition
	if(used < prevUsed) {
		m_nextQ.reset();
	}
}
void BufferedSerial::processTX( void) {
	if(m_phandler == NULL) return;
	if(m_phandler->gState != HAL_UART_STATE_READY) return;

	if(m_bytesTx > 0) {
		m_previousQ.drop(m_bytesTx);
		m_bytesTx = 0;
	}
	m_bytesTx = m_previousQ.getLinearReadBufferSize();
	if(m_bytesTx > 0) {
		m_bytesTx = m_bytesTx > SERIAL_TX_CHUNK_SIZE ? SERIAL_TX_CHUNK_SIZE : m_bytesTx;
		if( m_phandler->hdmatx) {
			HAL_StatusTypeDef handlerStatus = HAL_UART_Transmit_DMA( m_phandler, (unsigned char*)m_previousQ.getLinearReadBuffer(), m_bytesTx);
			m_error = halStatusToError(handlerStatus);
		}
	}
}

BufferedSerial::STM32SerialError BufferedSerial::halStatusToError(HAL_StatusTypeDef status) {
	switch(status) {
	case HAL_OK:
		return STM32SerialError::none;
	case HAL_ERROR:
		return STM32SerialError::handlerError;
	case HAL_BUSY:
		return STM32SerialError::handlerBusy;
	case HAL_TIMEOUT:
		return STM32SerialError::handlerTimeout;
	default:
		return STM32SerialError::handlerError;
	}
}

#ifdef ENABLE_SERIAL1
extern UART_HandleTypeDef huart1;
BufferedSerial BSerial1( huart1);
#endif
#ifdef ENABLE_SERIAL2
extern UART_HandleTypeDef huart2;
BufferedSerial BSerial2( &huart2);
#endif
#ifdef ENABLE_SERIAL3
extern UART_HandleTypeDef huart3;
BufferedSerial BSerial3( huart3);
#endif
#endif