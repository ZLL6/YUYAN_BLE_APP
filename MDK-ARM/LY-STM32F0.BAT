SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\Perl\site\bin;C:\Perl\bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files\TortoiseSVN\bin;C:\Program Files\VisualSVN Server\bin;C:\Program Files (x86)\Microsoft SQL Server\100\Tools\Binn\;C:\Program Files\Microsoft SQL Server\100\Tools\Binn\;C:\Program Files\Microsoft SQL Server\100\DTS\Binn\;C:\Windows\System32;C:\Qt\QtIFW-4.0.0\bin;C:\Strawberry\c\bin;C:\Strawberry\perl\site\bin;C:\Strawberry\perl\bin;D:\JDK_Path;D:\JDK_Path\bin;D:\JDK_Path\jre\bin;D:\LL\SDK\tools;D:\LL\SDK\platform-tools;D:\LL\gradle-5.4-all\gradle-5.4\bin;
SET CPU_TYPE=STM32F072CBTx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=LY-STM32F0
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\objects\startup_stm32f072._ia"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\can.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\jdq.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\tim.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\spi.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\w25qxx.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\bq769x0.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\simulatei2c.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\data.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\wkup.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\soc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\flash.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\rtc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\eventrecord.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\wwdg.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\uart.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\pbdata.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_it.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_gpio.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_rcc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_misc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_can.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_tim.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_spi.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_rtc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_pwr.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_flash.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_exti.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_syscfg.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_iwdg.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_usart.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f0xx_wwdg.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\system_stm32f0xx.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\Objects\LY-STM32F0.lnp"
"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\Objects\LY-STM32F0.axf" --i32combined --output ".\Objects\LY-STM32F0.hex"
C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bin -o  ./bin/YUYAN-U_V14.bin  ./Objects/LY-STM32F0.axf
