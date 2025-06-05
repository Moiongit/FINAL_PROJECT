# Hotel Reservation System

## Project Description
This is a console-based Hotel Reservation System implemented in C++. The system allows users to book rooms, manage reservations, and calculate bills using different billing strategies. There are two types of users: Admin and Customer. Admins can manage rooms, view all reservations, and change billing strategies, while customers can sign up, login, book rooms, view and cancel their reservations.

The system employs the Strategy design pattern for flexible bill calculation, supporting both standard billing and discounted billing (10% off for stays of 5 or more nights).

## Features
- User authentication for Admin and Customers
- Customer signup and login
- Room booking with availability check
- Different billing strategies (Standard and Discount)
- Admin functionalities:
  - View all reservations
  - Manage room details (add, edit rate, change availability)
  - Cancel any reservation
  - Change billing strategy dynamically
- Customer functionalities:
  - Book rooms
  - View and cancel own reservations
  - View available rooms
- Input validation and simple console UI for ease of use

## How to Run
1. Make sure you have a C++ compiler installed (e.g., g++, clang++).
2. Clone or download this repository.
3. Compile the program using the command:
   ```bash
   g++ -o HotelReservationSystem HRS.cpp

##Author
Moises Ceazar Del Mundo
