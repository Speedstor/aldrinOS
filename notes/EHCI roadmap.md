# EHCI roadmap
- the list of things that I need to do, and the things that I have done

### TODO
- [ ] detect Port Change Detect in USBSTS and handle wake-up events




## Chapters of Mindshare EHCI Whitepaper
- [ ] EHCI overview
- [ ] EHCI controller PCI and Memory Mapped I/O Registers
- [ ] EHCI register operational summary
- [ ] EHCI memory data structures. _use of the four memory structures set up by USB system software and used by the host controller to manage USB transactions_

## Chapters of Intel EHCI Specification For Universal Serial Bus
- [x] Architectural Overview
	- [ ] Interface Architecture
	- [ ] EHCI Schedule Data Structures
	- [ ] Root Hub Emulation
- [x] PCI Configuration Registers (USB)
	- [ ] PWRMGT
	- [ ] CLASSC (Class Code REGISTER)
	- [ ] USBBASE
	- [ ] SBRN
	- [ ] Frame Length Adjustment Register (FLADJ)
	- [ ] Port Wake Capability Register (PORTWAKECAP)
	- [ ] USBLEGSUP - USB Legacy Support Extended Capability
	- [ ] USBLEGCTLSTS - USB Legacy Support Control/Status
- [x] Host Controller Capability Registers
	- [x] CAPLENGTH - Capability Registers Length
	- [x] HCIVERSION - Host Controller Interface Version Number
	- [x] HCSPARAMS - Structural Parameters
	- [x] HCCPARAMS - CAPABILITY Parameters
	- [x] HCSP-PORTROUTE - Companion Port Route Description
- [ ] Host Controller Operational Registers
	- [ ] USBCMD - USB Command Register
	- [x] USBSTS - USB Status Register
	- [ ] USBINTR - USB Interrupt Enabled Register
	- [ ] FRINDEX - Frame Index Register
	- [ ] CTRLDSSEGMENT - Control Data Structure Segment Register
	- [ ] PERIODICLISTBASE - Periodic Frame List Base Address Register
	- [ ] ASYNCLISTADDR - Current Asynchronous List Address Register
	- [ ] CONFIGFLAG - Configure Flag Register
	- [x] PORTSC - Port Status and Control Register
- [ ] Data Structures
	- [ ] Periodic Frame List
	- [ ] Asynchronous List Queue Head Pointer
	- [ ] Isochronous (High-Speed) Transfer Descriptor (iTD)
	- [ ] Split Transaction Isochronous Transfer Descriptor (siTD)
	- [ ] Queue Element Transfer Descriptor (qTD)
	- [ ] Queue Head
	- [ ] Periodic Frame Span Traversal Node (FSTN)
- [ ] Operational Model
	- [x] Host Controller Initialization
	- [x] Port Routing and Control
	- [x] Suspend/Resume
	- [ ] Schedule Traversal Rules
	- [ ] Periodic Schedule Frame Boundaries vs Bus Frame Boundaries
	- [ ] Periodic Schedule
	- [ ] Managing Isochronous Transfers Using iTDs
	- [ ] Asynchronous Schedule
	- [ ] Operational Model for Nak Counter
	- [ ] Managing Control/Bulk/Interrupt Transfers via Queue Heads
	- [ ] Ping Control
	- [ ] Split Transactions
	- [ ] Host Controller Pause
	- [ ] Port Test Modes
	- [ ] Interrupts
- [ ] EHCI Extended Capabilities
	- [ ] EHCI Extended Capability: Pre-OS to OS Handoff Synchronization
- [ ] Appendix
