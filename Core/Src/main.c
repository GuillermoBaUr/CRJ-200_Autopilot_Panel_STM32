/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TX_BUFFER_SIZE 1
#define OUTPUT_DATA_SIZE 12
#define BUTTON_DATA_SIZE 17
#define DEBOUNCE_DELAY 25
#define SEND_DELAY 10
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
typedef struct {
    uint16_t pin;
    uint32_t lastInterruptTime;
} DebounceState;

typedef struct{
	GPIO_TypeDef* port;	//The GPIO port of the LED
	uint16_t pin;		//The pin number
	uint8_t state;		//The State of the LED or  BUTTON (0 = OFF, 1 = ON)
}OUTPUTS, INPUTS;

typedef struct LinkedListNode {				//structure of link list  for the QUEUE
	struct LinkedListNode* ptrNextNode;	   // pointer to next node in list (NULL if the end)
	uint8_t				   index;		 //The index of the element
	uint8_t				   sendData;	// The data to be sent
	uint8_t				   element;		  //The element to be send
} LinkedListNodeDef;

enum elements{
	ENCODER_FLAG,
	ENCODER_INDEX,
	ENCODER_DATA
};

OUTPUTS leds[OUTPUT_DATA_SIZE] = {
    {OUT_FD1_FD2_GPIO_Port, OUT_FD1_FD2_Pin, GPIO_PIN_SET},  // FD1 FD2 Led
    {OUT_AP_ENG_GPIO_Port, OUT_AP_ENG_Pin, GPIO_PIN_SET},    // AP ENG Led
    {OUT_XFR_GPIO_Port, OUT_XFR_Pin, GPIO_PIN_SET},          // XFR Led
    {OUT_TURB_GPIO_Port, OUT_TURB_Pin, GPIO_PIN_SET},        // TURB Led
    {OUT_SPD_GPIO_Port, OUT_SPD_Pin, GPIO_PIN_SET},          // SPD Led
    {OUT_APPR_GPIO_Port, OUT_APPR_Pin, GPIO_PIN_SET},        // APPR Led
    {OUT_BC_GPIO_Port, OUT_BC_Pin, GPIO_PIN_SET},            // BC Led
    {OUT_HDG_GPIO_Port, OUT_HDG_Pin, GPIO_PIN_SET},          // HDG Led
    {OUT_NAV_GPIO_Port, OUT_NAV_Pin, GPIO_PIN_SET},          // NAV Led
    {OUT_1_2_BANK_GPIO_Port, OUT_1_2_BANK_Pin, GPIO_PIN_SET},// 1/2 BANK Led
    {OUT_ALT_GPIO_Port, OUT_ALT_Pin, GPIO_PIN_SET},          // ALT Led
    {OUT_VS_GPIO_Port, OUT_VS_Pin, GPIO_PIN_SET},            // VS Led
};

INPUTS buttons[BUTTON_DATA_SIZE] = {
		{INP_FD1_FD2_GPIO_Port, INP_FD1_FD2_Pin, 1},		//FD1 FD2 Button
		{INP_ALT_GPIO_Port, INP_ALT_Pin, 1},				//ALT Button
		{INP_NAV_GPIO_Port, INP_NAV_Pin, 1},				//NAV Button
		{INP_HDG_GPIO_Port, INP_HDG_Pin, 1},				//HDG Button
		{INP_APRR_GPIO_Port, INP_APRR_Pin, 1},				//APPR Button
		{INP_TURB_GPIO_Port, INP_TURB_Pin, 1},				//TURB Button
		{INP_AP_DISC_GPIO_Port, INP_AP_DISC_Pin, 1},		//AP DISC Button
		{INP_AP_ENG_GPIO_Port, INP_AP_ENG_Pin, 1},			//AP ENG Button
		{INP_XFR_GPIO_Port, INP_XFR_Pin, 1},				//XFR Button
		{INP_SPD_GPIO_Port, INP_SPD_Pin, 1},				//SPD Button
		{INP_BC_GPIO_Port, INP_BC_Pin, 1},					//BC Button
		{INP_HDG_EPB_GPIO_Port, INP_HDG_EPB_Pin, 1},		//HDG Encoder's Button
		{INP_1_2_BANK_GPIO_Port, INP_1_2_BANK_Pin, 1},		//1/2 Button
		{INP_VS_GPIO_Port, INP_VS_Pin, 1},					//VS Button
		{ENC_SPD_EPB_GPIO_Port, ENC_SPD_EPB_Pin, 1},		//SPD Encoder's Button
		{ENC_CRS1_2_EPB_GPIO_Port, ENC_CRS1_2_EPB_Pin, 1},	//CRS1 Encoder's Button
		{ENC_ALT_EPB_GPIO_Port, ENC_ALT_EPB_Pin, 1},		//ALT Encoder's Button
};

DebounceState debounceStates[] = {
    {ENC_CRS1_B_Pin, 0},
    {ENC_SPD_B_Pin, 0},
    {ENC_HDG_B_Pin, 0},
    {ENC_ALT_B_Pin, 0},
    {ENC_VS_B_Pin, 0},
    {ENC_CRS2_B_Pin, 0}
};

volatile uint8_t encoderStorage[3] = {0};
volatile uint8_t received = 1;
volatile uint8_t tx_complete = 0;
uint16_t ledStatusRxBuffer = 0b111111111111;
uint32_t buttonStatusTxData = 0;
uint8_t uart_tx_buffer[BUTTON_DATA_SIZE];
LinkedListNodeDef* ptrHead = NULL;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
LinkedListNodeDef* putToQueue( uint8_t index, uint8_t data, uint8_t element, LinkedListNodeDef* ptrHead);
uint8_t transformData( uint8_t index, uint8_t data, uint8_t element);
void unpackData(uint8_t data);
LinkedListNodeDef* sendFromQueue(LinkedListNodeDef* ptrHead);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void unpackDataLED();
void updateLEDOutput();
void updateLeds();
void turnOffLED();
LinkedListNodeDef* readDatabuttons();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  //HAL_UART_Receive_IT(&huart2, uart_rx_buffer, OUTPUT_DATA_SIZE);
  turnOffLED();
  ptrHead = NULL;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  updateLeds();
	  ptrHead = readDatabuttons(ptrHead);
	  ptrHead = sendFromQueue(ptrHead);
	  if(encoderStorage[ENCODER_FLAG] == 1){
		  ptrHead = putToQueue(encoderStorage[ENCODER_INDEX], encoderStorage[ENCODER_DATA], 1, ptrHead);
		  encoderStorage[ENCODER_FLAG] = 0;
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, OUT_SPD_Pin|OUT_APPR_Pin|OUT_HDG_Pin|OUT_AP_ENG_Pin
                          |OUT_TURB_Pin|OUT_FD1_FD2_Pin|OUT_ALT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OUT_XFR_GPIO_Port, OUT_XFR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, OUT_VS_Pin|OUT_NAV_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, OUT_1_2_BANK_Pin|OUT_BC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : OUT_SPD_Pin OUT_APPR_Pin OUT_HDG_Pin OUT_AP_ENG_Pin
                           OUT_TURB_Pin OUT_FD1_FD2_Pin OUT_ALT_Pin */
  GPIO_InitStruct.Pin = OUT_SPD_Pin|OUT_APPR_Pin|OUT_HDG_Pin|OUT_AP_ENG_Pin
                          |OUT_TURB_Pin|OUT_FD1_FD2_Pin|OUT_ALT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : OUT_XFR_Pin */
  GPIO_InitStruct.Pin = OUT_XFR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OUT_XFR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : INP_ALT_Pin INP_FD1_FD2_Pin INP_AP_ENG_Pin INP_XFR_Pin
                           INP_SPD_Pin INP_VS_Pin INP_1_2_BANK_Pin ENC_SPD_EPB_Pin */
  GPIO_InitStruct.Pin = INP_ALT_Pin|INP_FD1_FD2_Pin|INP_AP_ENG_Pin|INP_XFR_Pin
                          |INP_SPD_Pin|INP_VS_Pin|INP_1_2_BANK_Pin|ENC_SPD_EPB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : OUT_VS_Pin OUT_NAV_Pin */
  GPIO_InitStruct.Pin = OUT_VS_Pin|OUT_NAV_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_CRS1_2_EPB_Pin INP_HDG_EPB_Pin INP_APRR_Pin INP_HDG_Pin */
  GPIO_InitStruct.Pin = ENC_CRS1_2_EPB_Pin|INP_HDG_EPB_Pin|INP_APRR_Pin|INP_HDG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : OUT_1_2_BANK_Pin OUT_BC_Pin */
  GPIO_InitStruct.Pin = OUT_1_2_BANK_Pin|OUT_BC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_CRS2_B_Pin ENC_CRS2_A_Pin ENC_SPD_B_Pin ENC_VS_B_Pin
                           ENC_CRS1_B_Pin ENC_CRS1_A_Pin ENC_HDG_A_Pin ENC_SPD_A_Pin
                           ENC_HDG_B_Pin */
  GPIO_InitStruct.Pin = ENC_CRS2_B_Pin|ENC_CRS2_A_Pin|ENC_SPD_B_Pin|ENC_VS_B_Pin
                          |ENC_CRS1_B_Pin|ENC_CRS1_A_Pin|ENC_HDG_A_Pin|ENC_SPD_A_Pin
                          |ENC_HDG_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_ALT_EPB_Pin INP_BC_Pin INP_TURB_Pin INP_AP_DISC_Pin */
  GPIO_InitStruct.Pin = ENC_ALT_EPB_Pin|INP_BC_Pin|INP_TURB_Pin|INP_AP_DISC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : ENC_ALT_B_Pin ENC_ALT_A_Pin */
  GPIO_InitStruct.Pin = ENC_ALT_B_Pin|ENC_ALT_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : ENC_VS_A_Pin */
  GPIO_InitStruct.Pin = ENC_VS_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ENC_VS_A_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : INP_NAV_Pin */
  GPIO_InitStruct.Pin = INP_NAV_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(INP_NAV_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	uint32_t currentTime = HAL_GetTick(); // Obtiene el tiempo actual en ms

	for (int8_t encoder = 0; encoder < 5; encoder++) {
		if (debounceStates[encoder].pin == GPIO_Pin) {
			if (currentTime - debounceStates[encoder].lastInterruptTime > DEBOUNCE_DELAY) {
				debounceStates[encoder].lastInterruptTime = currentTime;
				switch(GPIO_Pin){
					case ENC_CRS1_B_Pin:
						if(HAL_GPIO_ReadPin(GPIOB, ENC_CRS1_A_Pin)){
							encoderStorage[ENCODER_FLAG] = 1;
							encoderStorage[ENCODER_INDEX] = 0;
							encoderStorage[ENCODER_DATA] = 1;
						}else{
							encoderStorage[ENCODER_FLAG] = 1;
							encoderStorage[ENCODER_INDEX] = 0;
							encoderStorage[ENCODER_DATA] = 0;
						}
						break;
					case ENC_SPD_B_Pin:
						if(HAL_GPIO_ReadPin(GPIOB, ENC_SPD_A_Pin)){
							encoderStorage[ENCODER_FLAG] = 1;
							encoderStorage[ENCODER_INDEX] = 1;
							encoderStorage[ENCODER_DATA] = 1;
						}else{
							encoderStorage[ENCODER_FLAG] = 1;
							encoderStorage[ENCODER_INDEX] = 1;
							encoderStorage[ENCODER_DATA] = 0;
						}
						break;
					case ENC_HDG_B_Pin:
						if(HAL_GPIO_ReadPin(GPIOB, ENC_HDG_A_Pin)){
							encoderStorage[ENCODER_FLAG] = 1;
							encoderStorage[ENCODER_INDEX] = 2;
							encoderStorage[ENCODER_DATA] = 1;
						}else{
							encoderStorage[ENCODER_FLAG] = 1;
							encoderStorage[ENCODER_INDEX] = 2;
							encoderStorage[ENCODER_DATA] = 0;
						}
						break;
					case ENC_ALT_B_Pin:
						if(HAL_GPIO_ReadPin(GPIOC, ENC_ALT_A_Pin)){
							encoderStorage[ENCODER_FLAG] = 1;
							encoderStorage[ENCODER_INDEX] = 3;
							encoderStorage[ENCODER_DATA] = 1;
						}else{
							encoderStorage[ENCODER_FLAG] = 1;
							encoderStorage[ENCODER_INDEX] = 3;
							encoderStorage[ENCODER_DATA] = 0;
						}
						break;
					case ENC_VS_B_Pin:
						if(HAL_GPIO_ReadPin(GPIOA, ENC_VS_A_Pin)){
							encoderStorage[ENCODER_FLAG] = 1;
							encoderStorage[ENCODER_INDEX] = 4;
							encoderStorage[ENCODER_DATA] = 0;
						}else{
							encoderStorage[ENCODER_FLAG] = 1;
							encoderStorage[ENCODER_INDEX] = 4;
							encoderStorage[ENCODER_DATA] = 1;
						}
						break;
					case ENC_CRS2_B_Pin:
						if(HAL_GPIO_ReadPin(GPIOB, ENC_CRS2_A_Pin)){
							encoderStorage[ENCODER_FLAG] = 1;
							encoderStorage[ENCODER_INDEX] = 5;
							encoderStorage[ENCODER_DATA] = 1;
						}else{
							encoderStorage[ENCODER_FLAG] = 1;
							encoderStorage[ENCODER_INDEX] = 5;
							encoderStorage[ENCODER_DATA] = 0;
						}
						break;
				}
			}
		}
	}
}

// FUNCTION      : putToQueue()
// DESCRIPTION   : This function add a new element to the queue
// PARAMETERS    : LinkedListNodeDef* ptrHead - pointer to the head link list
//				   uint8_t index - index of the element
//				   uint8_t data - data from the element
//				   uint8_t element _ Type of element encoder or button
// RETURNS       : LinkedListNodeDef* - pointer to the head of the link list
LinkedListNodeDef* putToQueue( uint8_t index, uint8_t data, uint8_t element, LinkedListNodeDef* ptrHead) {

	LinkedListNodeDef* newNode = NULL; //Used for pointing to new node
	LinkedListNodeDef* currentNode = ptrHead; //Used for traversing

	newNode = (LinkedListNodeDef*)malloc(sizeof(LinkedListNodeDef)); //assigning memory address to the new node

	if (newNode == NULL) { // address validation
		return ptrHead;
	}

	newNode->ptrNextNode = NULL; //initializing structure elements
	newNode->element = element;
	newNode->index = index;
	newNode->sendData = data;

	if (ptrHead == NULL) { // if it is the first element
		return newNode;
	}

	while (currentNode->ptrNextNode != NULL) {
		currentNode = currentNode->ptrNextNode; // move to the last element
	}

	currentNode->ptrNextNode = newNode;


	return ptrHead;

}

// FUNCTION      : transformData()
// DESCRIPTION   : This function combine all the information in one variable to be send it
// PARAMETERS    : uint8_t index - index of the element
//				   uint8_t data - data from the element
//				   uint8_t element _ Type of element encoder or button
// RETURNS       : uint8_t - Data to be send
uint8_t transformData( uint8_t index, uint8_t data, uint8_t element) {

	uint8_t dataTransformed = index << 3;
	dataTransformed |= (data << 2);
	dataTransformed |= (element << 0);
	return  dataTransformed;
}

// FUNCTION      : sendFromQueue()
// DESCRIPTION   : This function send one data by time periods
// PARAMETERS    : LinkedListNodeDef* ptrHead - pointer to the head link list
// RETURNS       : LinkedListNodeDef* - pointer to the head of the link list
LinkedListNodeDef* sendFromQueue(LinkedListNodeDef* ptrHead) {

	LinkedListNodeDef* currentNode = ptrHead; //Used for traversing
	LinkedListNodeDef* nextPtrHead = ptrHead; //to save the next node address
	static uint32_t lastSendTime = 0;

	if(ptrHead == NULL){
		return NULL;
	}

	if((HAL_GetTick() - lastSendTime) > SEND_DELAY){
		nextPtrHead = currentNode->ptrNextNode; // Saving the next memory address
		uint8_t sendData = transformData(currentNode->index, currentNode->sendData, currentNode->element);
		HAL_UART_Transmit_IT(&huart2, &sendData, TX_BUFFER_SIZE);
		free(currentNode); //Free memory space
		currentNode = NULL;
		lastSendTime = HAL_GetTick();
	}

	return nextPtrHead;
}

// FUNCTION      : HAL_UART_RxCpltCallback()
// DESCRIPTION   : it is the interruption used when the microcontroller recive
//					information from the serial port
// PARAMETERS    :
//					UART_HandleTypeDef *huart - it is the UART instance used
// RETURNS       : nothing
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        received = 1;
    }
}

// FUNCTION      : unpackData()
// DESCRIPTION   : This function unpack the information form the simulator
//				   format to the led structure
// PARAMETERS    :
//					UART_HandleTypeDef *huart - it is the UART instance used
// RETURNS       : nothing
void unpackDataLED() {

	for (int8_t element = 0; element < OUTPUT_DATA_SIZE; element++) {

		leds[element].state = (ledStatusRxBuffer >> element) & 0x01;

	}

}

// FUNCTION      : updateLEDOutput()
// DESCRIPTION   : This function unpack the information form the simulator
//				   and update the LEDs
// PARAMETERS    : nothing
// RETURNS       : nothing
void updateLEDOutput(){

	static int16_t previousLedStatus = 0b111111111111;
	int16_t currentLedStatus = ledStatusRxBuffer;
	int16_t changedBits = previousLedStatus ^ currentLedStatus;
	int16_t pinState = 0;

	for (int8_t i = 0; i < OUTPUT_DATA_SIZE; i++) {
		if (changedBits & (1 << i)) {
			pinState = (currentLedStatus >> i) & 1;
			HAL_GPIO_WritePin(leds[i].port, leds[i].pin, pinState);
		}
	}
	 previousLedStatus = currentLedStatus;
}

// FUNCTION      : updateLeds()
// DESCRIPTION   : This function updates LED Outputs
// PARAMETERS    : nothing
// RETURNS       : nothing
void updateLeds(){
    if (received) {
        received = 0;
        updateLEDOutput();
        HAL_UART_Receive_IT(&huart2, (uint8_t*)&ledStatusRxBuffer, 2); // Restart UART reception AFTER processing data
    }
}


// FUNCTION      : updateLeds()
// DESCRIPTION   : This function Turn Off al LEDS
// PARAMETERS    : nothing
// RETURNS       : nothing
void turnOffLED(){


	for (int8_t i = 0; i < OUTPUT_DATA_SIZE; i++) {
	        HAL_GPIO_WritePin(leds[i].port, leds[i].pin, 1);
	    }
}

// FUNCTION      : readDatabuttons()
// DESCRIPTION   : This functions read the data in the buttons
//					and save in a QUEUE only if it is pressed
// PARAMETERS    : LinkedListNodeDef* ptrHead - pointer to the head link list
// RETURNS       : LinkedListNodeDef* - pointer to the head of the link list
LinkedListNodeDef* readDatabuttons(LinkedListNodeDef* ptrHead){

	static uint32_t lastTime[BUTTON_DATA_SIZE] = {0};
	LinkedListNodeDef* nodePtr = ptrHead;

	for(int8_t index = 0; index < BUTTON_DATA_SIZE; index++){

		int8_t currentState  = HAL_GPIO_ReadPin(buttons[index].port, buttons[index].pin);

		if(buttons[index].state != currentState){

			uint32_t currentTime = HAL_GetTick();

			if((currentTime - lastTime[index]) > DEBOUNCE_DELAY){
				if(currentState == 0 ){
					nodePtr = putToQueue(index, 1, 0, ptrHead);
				}
				buttons[index].state = currentState;
				lastTime[index] = currentTime;
				return nodePtr;
			}
		}
	}
	return nodePtr;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
