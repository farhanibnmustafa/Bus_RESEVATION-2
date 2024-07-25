#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <conio.h>
#include <termios.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h> // Required for Sleep() on Windows
#else
#include <unistd.h> // Required for sleep() on Unix-based systems
#endif

// Define function to clear console screen
void clearScreen() {
    #ifdef _WIN32 // For Windows
    system("cls");
    #else // For Linux and macOS
    system("clear");
    #endif
}

// Function to disable echoing in terminal
void disableEcho() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~ECHO; // Turn off echoing
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Function to enable echoing in terminal
void enableEcho() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ECHO; // Turn on echoing
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
#define MAX_USERS 100
#define MAX_BOOKINGS 100
#define ROUTE_COUNT 3
#define MAX_ROUTES 3
#define MAX_BUSES 3
#define MAX_SEATS 40
#define MAX_LOGIN_ATTEMPTS 3
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
// #define MAX_BUSES_PER_ROUTE 3
#define SEAT_COUNT 10
#define MAX_SEATS_BOOKABLE 4
#define FILENAME_USERS "users.dat"
#define FILENAME_BUSES "buses.dat"
#define FILENAME_BOOKINGS "bookings.dat"
#define FILENAME_ADMIN_LOG "admin_log.dat"
// Admin credentials
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "admin123"
const char SECURITY_QUESTION[] = "What is the capital of France?";
const char SECURITY_ANSWER[] = "Paris";

typedef struct {
    char route[50];
    char bus[50];
    int seats[SEAT_COUNT];
    int price;
} Bus;

typedef struct {
    char username[50];
    char password[50];
    char name[100];
    char email[100];
    char phone[15];
    char security_question[200];
    char security_answer[100];
    int is_logged_in;
} User;

typedef struct {
    char route[50];
    char bus[50];
    int seat_numbers[MAX_SEATS_BOOKABLE];
    int seat_count;
    char username[50];
    char date[11];
} Booking;

User users[MAX_USERS];
int user_count = 0;

Booking bookings[MAX_BOOKINGS];
int booking_count = 0;

Bus buses[MAX_ROUTES][MAX_BUSES];
int buses_per_route[MAX_ROUTES] = {0};

void load_users();
void save_users();
void load_buses();
void save_buses();
void load_bookings();
void save_bookings();
int find_user_by_username(const char *username);
int login();
void register_user();
void sign_out(int user_idx);
void view_profile(int user_idx);
void update_profile(int user_idx);
void make_reservation(int user_idx);
void cancel_booking(int user_idx);
void view_booking_details(int user_idx);
void reset_password();
void adminMenu();
void add_bus_details();
void delete_bus_details();
void update_bus_details();
void update_seat_status();
void set_bus_prices();
void view_admin_log();
void log_booking(Booking *booking);
void user_menu(int user_idx);

void signoutAdmin();
void resetAdminPassword();
void writePasswordToFile(const char *password);
void readPasswordFromFile();

void load_users() {
    FILE *file = fopen(FILENAME_USERS, "rb");
    if (file) {
        fread(&user_count, sizeof(int), 1, file);
        fread(users, sizeof(User), user_count, file);
        fclose(file);
    }
}

void save_users() {
    FILE *file = fopen(FILENAME_USERS, "wb");
    fwrite(&user_count, sizeof(int), 1, file);
    fwrite(users, sizeof(User), user_count, file);
    fclose(file);
}

void load_buses() {
    FILE *file = fopen("buses.dat", "rb");
    if (file) {
        fread(buses, sizeof(Bus), MAX_ROUTES * MAX_BUSES, file);
        fclose(file);
        // Initialize the buses_per_route array
        for (int i = 0; i < MAX_ROUTES; i++) {
            buses_per_route[i] = 0;
            for (int j = 0; j < MAX_BUSES; j++) {
                if (buses[i][j].bus[0] != '\0') { // Check if bus name is not empty
                    buses_per_route[i]++;
                }
            }
        }
    } else {
        // Initialize buses if file does not exist
        for (int i = 0; i < MAX_ROUTES; i++) {
            buses_per_route[i] = MAX_BUSES;
            for (int j = 0; j < MAX_BUSES; j++) {
                sprintf(buses[i][j].route, "Route%d", i + 1);
                sprintf(buses[i][j].bus, "Bus%d", j + 1);
                for (int k = 0; k < MAX_SEATS; k++) {
                    buses[i][j].seats[k] = 0;
                }
                buses[i][j].price = 50; // Default price for each seat
            }
        }
        save_buses();
    }
}

void save_buses() {
    FILE *file = fopen("buses.dat", "wb");
    fwrite(buses, sizeof(Bus), MAX_ROUTES * MAX_BUSES, file);
    fclose(file);
}


void load_bookings() {
    FILE *file = fopen(FILENAME_BOOKINGS, "rb");
    if (file) {
        fread(&booking_count, sizeof(int), 1, file);
        fread(bookings, sizeof(Booking), booking_count, file);
        fclose(file);
    }
}

void save_bookings() {
    FILE *file = fopen(FILENAME_BOOKINGS, "wb");
    fwrite(&booking_count, sizeof(int), 1, file);
    fwrite(bookings, sizeof(Booking), booking_count, file);
    fclose(file);
}

int find_user_by_username(const char *username) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return i;
        }
    }
    return -1; // User not found
}
void register_user() {
    if (user_count >= MAX_USERS) {
        printf("                  *User limit reached. Cannot register more users*\n");
        return;
    }

    User new_user;
printf("                       ----------------------------------------------------------\n");
sleep(1);
printf("                       |              Hey,What's your name?                     |\n\n");
sleep(1);  
printf("                       |              Please create an account to continue..    |\n");
sleep(1);
printf("                       -----------------------------------------------------------\n\n\n");
    printf("                                   Enter username: ");
    scanf("%s", new_user.username);
    if (find_user_by_username(new_user.username) != -1) {
        printf("                       *Username already exists. Please choose a different username*\n");
        return;  
    }  
    printf("\n\n                                   Enter password: ");
    disableEcho();
    scanf("%s", new_user.password);  
    enableEcho();
    printf("\n \n                                   Enter your name: ");
    scanf("%s", new_user.name);  
    printf("\n\n                                   Enter your email: ");
    scanf("%s", new_user.email);  
    printf("\n\n                                   Enter phone: ");
    scanf("%s", new_user.phone);  
    printf("\n\n                                   Enter security question: ");
    getchar(); // to consume newline left by previous input
    fgets(new_user.security_question, 200, stdin);
    new_user.security_question[strcspn(new_user.security_question, "\n")] = '\0'; // remove newline
    printf("\n\n                                   Enter security answer: ");
    fgets(new_user.security_answer, 100, stdin);
    new_user.security_answer[strcspn(new_user.security_answer, "\n")] = '\0'; // remove newline

    new_user.is_logged_in = 0;
    users[user_count++] = new_user;
    save_users();
    printf("\n\n\n                                 |  User registered successfully! |\n");
   getchar();
   clearScreen();
}


int login() {
    char username[50], password[50];
        printf("                     ------------------------------------------------------------------\n\n");
 sleep(1);
 printf("                     |    **************** Welcome to the Journey ******************   |\n\n");
 sleep(1);
 printf("                     -------------------------------------------------------------------\n\n");
    printf("                                               Username: ");
    scanf("%s", username);
    printf("\n\n                                           Password: ");
    disableEcho();
    scanf("%s", password);
 enableEcho();
    int user_idx = find_user_by_username(username);
    if (user_idx != -1 && strcmp(users[user_idx].password, password) == 0) {
        users[user_idx].is_logged_in = 1;
        printf("Login successful!\n");
        user_menu(user_idx);
        return user_idx;
    } else {
        printf("Invalid username or password.\n");
        return -1;
    }
}

void sign_out(int user_idx) {
    users[user_idx].is_logged_in = 0;
    printf("Signed out successfully.\n");
}

void reset_password() {
    char username[50], security_answer[100];
    printf("Enter username: ");
    scanf("%s", username);

    int user_idx = find_user_by_username(username);
    if (user_idx == -1) {
        printf("User not found.\n");
        return;
    }

    printf("Security Question: %s\n", users[user_idx].security_question);
    printf("Enter security answer: ");
    getchar(); // to consume newline left by previous input
    fgets(security_answer, 100, stdin);
    security_answer[strcspn(security_answer, "\n")] = '\0'; // remove newline

    if (strcmp(users[user_idx].security_answer, security_answer) == 0) {
        printf("Enter new password: ");
        scanf("%s", users[user_idx].password);
        save_users();
        printf("Password reset successfully!\n");
    } else {
        printf("Incorrect security answer.\n");
    }
}

void view_profile(int user_idx) {
    User user = users[user_idx];
         printf("                     -----------------------------------------------------------------------------\n\n");
 sleep(1);
 printf("                     |   ****************    Profile Information    ******************   |\n\n");
 sleep(2);
 printf("                     -----------------------------------------------------------------------------\n\n");
    printf(" \n\n                        Name: %s\n", user.name);
    printf("  \n\n                       Email: %s\n", user.email);
    printf("   \n\n                      Phone: %s\n", user.phone);
}

void update_profile(int user_idx) {
    User *user = &users[user_idx];
            printf("                     -----------------------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************    Update Profile Information    ******************   |\n\n");
sleep(2);
printf("                     -----------------------------------------------------------------------------\n\n");

    printf(" \n\n                       Enter new name: ");
    scanf("%s", user->name);
    printf(" \n\n                       Enter new email: ");
    scanf("%s", user->email);
    printf("\n\n                        Enter new phone: ");
    scanf("%s", user->phone);
    save_users();
    printf("Profile updated successfully!\n");
    getchar();
    clearScreen();
}

void make_reservation(int user_idx) {
    int route_choice, bus_choice, seat_choices[MAX_SEATS], seat_count, total_cost, current_step = 0;
    char date[11];

    while (1) {
        switch (current_step) {
            case 0:
                     printf("                     -----------------------------------------------------------------------------\n\n");
 sleep(1);
 printf("                     |   ****************    Available Routes    ******************   |\n\n");
 sleep(2);
 printf("                     -----------------------------------------------------------------------------\n\n");
                for (int i = 0; i < MAX_ROUTES; i++) {
                    printf("                          %d. Route%d\n", i + 1, i + 1);
                }
                sleep(1);
                printf("\n\n                          Choose a route (0 to go back): ");
                scanf("%d", &route_choice);
                route_choice--;

                if (route_choice == -1) return;
                if (route_choice < 0 || route_choice >= MAX_ROUTES) {
                    printf("                           * Invalid route choice *\n");
                } else {
                    current_step = 1;
                }
                break;

            case 1:
            clearScreen();
                                printf("                     -----------------------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************    Available Buses    ******************   |\n\n");
sleep(2);
printf("                     -----------------------------------------------------------------------------\n\n");
                for (int i = 0; i < buses_per_route[route_choice]; i++) {
                    printf("                      %d. %s\n", i + 1, buses[route_choice][i].bus);
                }
                printf("\n\n                       Choose a bus (0 to go back): ");
                scanf("%d", &bus_choice);
                bus_choice--;

                if (bus_choice == -1) {
                    current_step = 0;
                    continue;
                }
                if (bus_choice < 0 || bus_choice >= buses_per_route[route_choice]) {
                    printf(" \n                   * Invalid bus choice *\n");
                } else {
                    current_step = 2;
                }
                break;

            case 2:
            clearScreen();
                printf("\n\n                      Enter the date for the reservation (dd-mm-yyyy): ");
                scanf("%s", date);
                current_step = 3;
                break;

            case 3:
            clearScreen();
                printf("\n\n                    - - - Available Seats (0 = available, 1 = booked) - - -\n\n");
                for (int i = 0; i < MAX_SEATS; i++) {
                    if (buses[route_choice][bus_choice].seats[i] == 0) {
                        printf("                   %d\t\t ", i + 1);
                    }
                }
                printf("\n\n                         Choose up to %d seats (0 to go back): ", MAX_SEATS_BOOKABLE);
                seat_count = 0;
                for (int i = 0; i < MAX_SEATS_BOOKABLE; i++) {
                    scanf("%d", &seat_choices[i]);
                    if (seat_choices[i] == 0) break;
                    seat_choices[i]--;
                    if (seat_choices[i] < 0 || seat_choices[i] >= MAX_SEATS ||
                        buses[route_choice][bus_choice].seats[seat_choices[i]] == 1) {
                        printf("                  * Invalid seat choice *\n");
                        seat_count = 0;
                        break;
                    }
                    seat_count++;
                }
                if (seat_count == 0) {
                    current_step = 2;
                    continue;
                }
                current_step = 4;
                break;

            case 4:
            clearScreen();
                total_cost = seat_count * buses[route_choice][bus_choice].price;
                printf(" \n\n                   Total Ticket Cost: %d\n", total_cost);
                printf("                        Confirm booking (1: Yes, 0: No): ");
                int confirm;
                scanf("%d", &confirm);
                if (confirm == 1) {
                    for (int i = 0; i < seat_count; i++) {
                        buses[route_choice][bus_choice].seats[seat_choices[i]] = 1;
                    }
                    Booking new_booking;
                    strcpy(new_booking.username, users[user_idx].username);
                    strcpy(new_booking.route, buses[route_choice][bus_choice].route);
                    strcpy(new_booking.bus, buses[route_choice][bus_choice].bus);
                    new_booking.seat_count = seat_count;
                    strcpy(new_booking.date, date);
                    for (int i = 0; i < seat_count; i++) {
                        new_booking.seat_numbers[i] = seat_choices[i];
                    }
                    bookings[booking_count++] = new_booking;
                    log_booking(&new_booking);
                    printf("\n               Booking successful. \n\nSeats booked: ");
                    for (int i = 0; i < seat_count; i++) {
                        printf("%d ", seat_choices[i] + 1);
                    }
                    printf("\n                Total ticket Cost: %d\n", total_cost);
                }
                return;
        }
    }
}

void cancel_booking(int user_idx) {
    clearScreen();
    int booking_choice;
                           printf("                     -----------------------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************   Your Bookings    ******************   |\n\n");
sleep(2);
printf("                     -----------------------------------------------------------------------------\n\n");
    for (int i = 0; i < booking_count; i++) {
        if (strcmp(bookings[i].username, users[user_idx].username) == 0) {
            printf("                  %d. Route: %s\n, Bus: %s\n, Date: %s\n, Seats: ", i + 1, bookings[i].route, bookings[i].bus, bookings[i].date);
            for (int j = 0; j < bookings[i].seat_count; j++) {
                printf("%d ", bookings[i].seat_numbers[j] + 1);
            }
            printf("\n");
        }
    }
    printf("\n\n                         Choose a booking to cancel (0 to go back): ");
    scanf("%d", &booking_choice);
    booking_choice--;

    if (booking_choice == -1) return;
    if (booking_choice < 0 || booking_choice >= booking_count || strcmp(bookings[booking_choice].username, users[user_idx].username) != 0) {
        printf("\n                        * Invalid booking choice *\n");
        return;
    }

    for (int i = 0; i < bookings[booking_choice].seat_count; i++) {
        int route_index, bus_index;
        sscanf(bookings[booking_choice].route, "Route%d", &route_index);
        route_index--;
        for (int j = 0; j < buses_per_route[route_index]; j++) {
            if (strcmp(buses[route_index][j].bus, bookings[booking_choice].bus) == 0) {
                bus_index = j;
                break;
            }
        }
        buses[route_index][bus_index].seats[bookings[booking_choice].seat_numbers[i]] = 0;
    }

    for (int i = booking_choice; i < booking_count - 1; i++) {
        bookings[i] = bookings[i + 1];
    }
    booking_count--;
    printf("\n\n                         - - Booking canceled successfully - -\n");
}

void view_booking_details(int user_idx) {
    clearScreen();
                            printf("                     -----------------------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************   Booking Details    ******************   |\n\n");
sleep(2);
printf("                     -----------------------------------------------------------------------------\n\n");
    for (int i = 0; i < booking_count; i++) {
        if (strcmp(bookings[i].username, users[user_idx].username) == 0) {
                                       printf("                     ------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************  Ticket    ******************   |\n\n");
sleep(1);
printf("                     -----------------------------------------------------------------------------\n\n");
            printf("                 Route: %s\n, Bus: %s\n, Seats: ", bookings[i].route, bookings[i].bus);
            for (int j = 0; j < bookings[i].seat_count; j++) {
                printf("      %d ", bookings[i].seat_numbers[j]);
            }
            printf("\nDate: %s\n", bookings[i].date);
        }
    }
}


// int admin_login() {
//     char username[50], password[50];
//     printf("Admin Username: ");
//     scanf("%s", username);
//     printf("Admin Password: ");
//     scanf("%s", password);

//     if (strcmp(username, ADMIN_USERNAME) == 0 && strcmp(password, ADMIN_PASSWORD) == 0) {
//         printf("Admin login successful!\n");
//         return 1;
//     } else {
//         printf("Invalid admin username or password.\n");
//         return 0;
//     }
// }

// void admin_logout() {
//     printf("Admin logged out successfully.\n");
// }

int adminLogin() {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    int attempts = 0;

    clearScreen();

    do {
        printf("\n                                             =============================================");
        printf("\n                           **********************************  Administrator Login   **********************************");
        printf("\n                                             =============================================");

        printf("\n\n                                                          Username: ");
        scanf("%s", username);
         printf("\n                                                           Password: ");
         disableEcho(); // Disable echoing
        scanf("%s", password);
         enableEcho(); // Enable echoing

        // Check if username and password are correct
        if (strcmp(username, ADMIN_USERNAME) == 0 && strcmp(password, ADMIN_PASSWORD) == 0) {
            printf("\n\n                     |                   Login successful!           |\n");
            getchar();
            clearScreen();
            return 1;
            clearScreen();
        } else {
            attempts++;
            printf("\n\n\n                                                  * Incorrect username or password!");

            if (attempts < MAX_LOGIN_ATTEMPTS) {
                printf(" You have %d attempts left.", MAX_LOGIN_ATTEMPTS - attempts);
            } else {
                printf(" You have reached the maximum number of login attempts.");
                char choice;
                printf("\n\n\n                                                  * Do you want to reset your password? (Y/N): ");
                scanf(" %c", &choice);
                if (choice == 'Y' || choice == 'y') {
                    clearScreen();
                    resetAdminPassword();
                    return 0;
                } else {
                    return 0;
                }
            }
        }
    } while (attempts < MAX_LOGIN_ATTEMPTS);

    return 0;
}

void resetAdminPassword() {
    char answer[100];
    char new_password[MAX_PASSWORD_LENGTH];

    clearScreen();

    printf("\n\n\n                                                  * To reset your password, please answer the following security question:");
    printf("\n                                                  * %s\n", SECURITY_QUESTION);
    printf("\n\n\n                                                  * Your Answer: ");
    scanf("%s", answer);

    if (strcmp(answer, SECURITY_ANSWER) == 0) {
        printf("\n\n\n                                                  * Security question verified! Please enter your new password: ");
        scanf("%s", new_password);

        // Update the password
        strcpy(ADMIN_PASSWORD, new_password);
        writePasswordToFile(new_password);
        printf("\n\n\n                                                  * Password updated successfully!");
    } else {
        printf("\n\n\n                                                  * Incorrect answer to security question. Password reset failed.");
    }

    printf("\n\n\n                                                  * Press Enter to continue...");
    while (getchar() != '\n');
    getchar();

    // Go back to login phase
    adminLogin();
    clearScreen();
    adminMenu();
}
// Function to write password to file
void writePasswordToFile(const char *password) {
    FILE *file = fopen("admin_password.txt", "w");
    if (file != NULL) {
        fputs(password, file);
        fclose(file);
    } else {
        printf("\n\n\n                                                  * Error writing password to file.");
    }
}

// Function to read password from file
void readPasswordFromFile() {
    FILE *file = fopen("admin_password.txt", "r");
    if (file != NULL) {
        fgets(ADMIN_PASSWORD, MAX_PASSWORD_LENGTH, file);
        fclose(file);
    } else {
        printf("\n\n\n                                                  * Error reading password from file.");
    }
}
void signoutAdmin(){
   clearScreen();
    printf("\n                            --------------------------- Signing out... --------------------------");
    getchar();
    getchar();
    clearScreen(); // Clear the screen
    adminLogin(); // Go back to the admin login interface
    clearScreen(); // Clear the screen after login
    adminMenu(); // Display the admin menu
}

// void admin_menu() {
//     int choice;
//     while (1) {
//         printf("\nAdmin Menu\n");
//         printf("1. Add Bus Details\n");
//         printf("2. Delete Bus Details\n");
//         printf("3. Update Bus Details\n");
//         printf("4. Update Seat Status\n");
//         printf("5. Set Bus Prices\n");
//         printf("6. View Booking Log\n");
//         printf("7. Exit Admin Menu\n");
//         printf("Enter your choice: ");
//         scanf("%d", &choice);

//         switch (choice) {
//             case 1:
//                 add_bus_details();
//                 break;
//             case 2:
//                 delete_bus_details();
//                 break;
//             case 3:
//                 update_bus_details();
//                 break;
//             case 4:
//                 update_seat_status();
//                 break;
//             case 5:
//                 set_bus_prices();
//                 break;
//             case 6:
//                 view_admin_log();
//                 break;
//             case 7:
//                 return;
//             default:
//                 printf("Invalid choice. Try again.\n");
//         }
//     }
// }

void add_bus_details() {
    int route_choice;
                printf("                     -------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************   Available Routes    ******************   |\n\n");
sleep(2);
printf("                     -----------------------------------------------------------------\n\n");

    for (int i = 0; i < MAX_ROUTES; i++) {
        printf("                                         %d. Route%d\n", i + 1, i + 1);
    }
    printf(" \n                                          Choose a route: ");
    scanf("%d", &route_choice);
    route_choice--;

    if (route_choice < 0 || route_choice >= MAX_ROUTES) {
        printf("                                            *Invalid route choice*\n");
        return;
    }

    if (buses_per_route[route_choice] >= MAX_BUSES) {
        printf("                                    *Maximum number of buses for this route reached*\n");
        return;
    }

    int bus_index = buses_per_route[route_choice];
    printf("                                                Enter bus name: ");
    scanf("%s", buses[route_choice][bus_index].bus);
    strcpy(buses[route_choice][bus_index].route, "Route"); // Adjust this as needed

    for (int i = 0; i < MAX_SEATS; i++) {
        buses[route_choice][bus_index].seats[i] = 0;
    }
    buses_per_route[route_choice]++;
    printf("                                         - - Bus details added successfully - -\n");
}

void delete_bus_details() {
    int route_choice, bus_choice;
                    printf("                     -----------------------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************   Available Routes    ******************   |\n\n");
sleep(2);
printf("                     -----------------------------------------------------------------------------\n\n");
    for (int i = 0; i < MAX_ROUTES; i++) {
        printf("                                       %d. Route%d\n", i + 1, i + 1);
    }
    printf("\n\n                                        Choose a route: ");
    scanf("%d", &route_choice);
    route_choice--;

    if (route_choice < 0 || route_choice >= MAX_ROUTES) {
        printf("                                     * Invalid route choice *\n");
        return;
    }

    if (buses_per_route[route_choice] == 0) {
        printf(" \n                             - - No buses available for this route - -\n");
        return;
    }
                printf("                     -----------------------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************   Available Buses    ******************   |\n\n");
sleep(2);
printf("                     -----------------------------------------------------------------------------\n\n");
    for (int i = 0; i < buses_per_route[route_choice]; i++) {
        printf("                                   %d. %s\n", i + 1, buses[route_choice][i].bus);
    }
    printf("\n\n                                    Choose a bus to delete: ");
    scanf("%d", &bus_choice);
    bus_choice--;

    if (bus_choice < 0 || bus_choice >= buses_per_route[route_choice]) {
        printf("                                     * Invalid bus choice *\n");
        return;
    }

    for (int i = bus_choice; i < buses_per_route[route_choice] - 1; i++) {
        buses[route_choice][i] = buses[route_choice][i + 1];
    }
    buses_per_route[route_choice]--;
    printf("\n\n                                - - Bus details deleted successfully - -\n");
}


void update_bus_details() {
    int route_choice, bus_choice;
                    printf("                     -----------------------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************   Available Routes    ******************   |\n\n");
sleep(2);
printf("                     -----------------------------------------------------------------------------\n\n");
    for (int i = 0; i < MAX_ROUTES; i++) {
        printf("                                        %d. Route%d\n", i + 1, i + 1);
    }
    printf(" \n\n                                        Choose a route: ");
    scanf("%d", &route_choice);
    route_choice--;

    if (route_choice < 0 || route_choice >= MAX_ROUTES) {
        printf("                                      * Invalid route choice *\n");
        return;
    }

    if (buses_per_route[route_choice] == 0) {
        printf("                                 - - No buses available for this route - -\n");
        return;
    }
                    printf("                     -----------------------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************   Available Buses    ******************   |\n\n");
sleep(2);
printf("                     -----------------------------------------------------------------------------\n\n");
    for (int i = 0; i < buses_per_route[route_choice]; i++) {
        printf("                               %d. %s\n", i + 1, buses[route_choice][i].bus);
    }
    printf("\n\n                               Choose a bus to update: ");
    scanf("%d", &bus_choice);
    bus_choice--;

    if (bus_choice < 0 || bus_choice >= buses_per_route[route_choice]) {
        printf("                                * Invalid bus choice *\n");
        return;
    }

    printf("\n\n                                    Enter new bus name: ");
    scanf("%s", buses[route_choice][bus_choice].bus);
    printf("\n\n                          - - Bus details updated successfully - -\n");
}

void update_seat_status() {
   clearScreen();
    int route_choice, bus_choice, seat_choice;
                        printf("                     -----------------------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************   Available Routes    ******************   |\n\n");
sleep(2);
printf("                     -----------------------------------------------------------------------------\n\n");
    for (int i = 0; i < MAX_ROUTES; i++) {
        printf("                                       %d. Route%d\n", i + 1, i + 1);
    }
    printf("\n\n                                       Choose a route: ");
    scanf("%d", &route_choice);
    route_choice--;

    if (route_choice < 0 || route_choice >= MAX_ROUTES) {
        printf("\n                                     * Invalid route choice *\n");
        return;
    }

    if (buses_per_route[route_choice] == 0) {
        printf("\n\n                            - - No buses available for this route - -\n");
        return;
    }
                        printf("                     -----------------------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************  Available Buses    ******************   |\n\n");
sleep(2);
printf("                     -----------------------------------------------------------------------------\n\n");
    for (int i = 0; i < buses_per_route[route_choice]; i++) {
        printf("                      %d. %s\n", i + 1, buses[route_choice][i].bus);
    }
    printf("\n\n                                 Choose a bus: ");
    scanf("%d", &bus_choice);
    bus_choice--;

    if (bus_choice < 0 || bus_choice >= buses_per_route[route_choice]) {
        printf("                                * Invalid bus choice *\n");
        return;
    }
    sleep(1);
    printf("                              Available Seats (0 = available, 1 = booked):\n");
    for (int i = 0; i < MAX_SEATS; i++) {
        printf("                           %d [%d] \n ", i + 1, buses[route_choice][bus_choice].seats[i]);
    }
    printf("\n\n                               Choose a seat to update status: ");
    scanf("%d", &seat_choice);
    seat_choice--;

    if (seat_choice < 0 || seat_choice >= MAX_SEATS) {
        printf("\n\n                              * Invalid seat choice *\n");
        return;
    }

    printf("\n\n                                Enter new seat status (0 = available, 1 = booked): ");
    int new_status;
    scanf("%d", &new_status);

    if (new_status != 0 && new_status != 1) {
        printf("\n\n                          * Invalid status choice *\n");
        return;
    }

    buses[route_choice][bus_choice].seats[seat_choice] = new_status;

    // Ensure to save buses data to a file for persistence
    save_buses();
    printf("\n\n                         - - Seat status updated successfully - -\n");
}


void set_bus_prices() {
    clearScreen();
    int route_choice, bus_choice, price;
                            printf("                     -----------------------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************  Available Routes    ******************   |\n\n");
sleep(2);
printf("                     -----------------------------------------------------------------------------\n\n");
    for (int i = 0; i < MAX_ROUTES; i++) {
        printf("                                 %d. Route%d\n", i + 1, i + 1);
    }
    printf("\n\n                                 Choose a route: ");
    scanf("%d", &route_choice);
    route_choice--;

    if (route_choice < 0 || route_choice >= MAX_ROUTES) {
        printf("\n\n                             * Invalid route choice *\n");
        return;
    }

    if (buses_per_route[route_choice] == 0) {
        printf("                              - - No buses available for this route - -\n");
        return;
    }
                            printf("                     -----------------------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************  Available Buses    ******************   |\n\n");
sleep(2);
printf("                     -----------------------------------------------------------------------------\n\n");
    for (int i = 0; i < buses_per_route[route_choice]; i++) {
        printf("                              %d. %s\n", i + 1, buses[route_choice][i].bus);
    }
    printf("\n\n                              Choose a bus: ");
    scanf("%d", &bus_choice);
    bus_choice--;

    if (bus_choice < 0 || bus_choice >= buses_per_route[route_choice]) {
        printf("   \n                    * Invalid bus choice *\n");
        return;
    }

    printf("\n\n                          Enter new price for all seats: ");
    scanf("%d", &price);
    buses[route_choice][bus_choice].price = price;
    printf("\n\n                    - - Bus prices updated successfully - -\n");
}


void view_admin_log() {
    FILE *file = fopen(FILENAME_ADMIN_LOG, "rb");
    if (!file) {
        printf("\n                    - - No booking logs available - -\n");
        return;
    }

    Booking booking;
                               printf("                     -----------------------------------------------------------------------------\n\n");
sleep(1);
printf("                     |   ****************  Booking Logs    ******************   |\n\n");
sleep(2);
printf("                     -----------------------------------------------------------------------------\n\n");
    while (fread(&booking, sizeof(Booking), 1, file)) {
        printf("Username: %s\n, Route: %s\n, Bus: %s\n, Date: %s\n, Seats: ", booking.username, booking.route, booking.bus, booking.date);
        for (int i = 0; i < booking.seat_count; i++) {
            printf("%d ", booking.seat_numbers[i]);
        }
        printf("\n");
    }
    fclose(file);
}

void log_booking(Booking *booking) {
    FILE *file = fopen(FILENAME_ADMIN_LOG, "ab");
    fwrite(booking, sizeof(Booking), 1, file);
    fclose(file);
}

// void user_menu(int user_idx) {
//     int choice;
//     while (users[user_idx].is_logged_in) {
//         printf("\nUser Menu\n");
//         printf("1. View Profile\n");
//         printf("2. Update Profile\n");
//         printf("3. Make Reservation\n");
//         printf("4. View Booking Details\n");
//         printf("5. Cancel Booking\n");
//         printf("6. Sign Out\n");
//         printf("7. Reset Password\n");
//         printf("Enter your choice: ");
//         scanf("%d", &choice);

//         switch (choice) {
//             case 1:
//                 // clearScreen();
//                 view_profile(user_idx);
//                 break;
//             case 2:
//                 // clearScreen();
//                 update_profile(user_idx);
//                 break;
//             case 3:
//                 // clearScreen();
//                 make_reservation(user_idx);
//                 break;
//             case 4:
//                 // clearScreen();
//                 view_booking_details(user_idx);
//                 break;
//             case 5:
//                 // clearScreen();
//                 cancel_booking(user_idx);
//                 break;
//             case 6:
//                 // clearScreen();
//                 sign_out(user_idx);
//                 break;
//             case 7:
//                 // clearScreen();
//                 reset_password();
//                 break;
//             default:
//                 clearScreen();
//                 printf("Invalid choice. Try again.\n");
//         }
//     }
// }

int main() {
    clearScreen(); // Clear screen when program starts
    load_users();
    load_buses();
    load_bookings();

    int choice;
    while (1) {
        printf("                     -----------------------------------------------------------------------------\n\n");
    sleep(1);
    printf("                     |   **************** Welcome to Bus Reservation System ******************   |\n\n");
    sleep(2);
    printf("                     -----------------------------------------------------------------------------\n\n");
    sleep(1);
    printf("                     |                      1. Register User                                     |\n\n");
    sleep(1);
    printf("                     |                      2. User Login                                        |\n\n");
    sleep(1);
    printf("                     |                      3. Admin Login                                       |\n\n");
    sleep(1);
    printf("                     |                      4. Exit                                              |\n\n");
    sleep(1);
    printf("                     -----------------------------------------------------------------------------\n\n");
    sleep(2);
    printf("                                            Your choice: ");
    scanf("%d", &choice);
    
        switch (choice) {
            case 1:
            clearScreen();
                register_user();
                break;
            case 2:
            clearScreen();
                login();
                break;
            case 3:
                // clearScreen();
                if (adminLogin()) {
                    adminMenu();
                   // signoutAdmin();
                }
                break;
            case 4:
                save_users();
                save_bookings();
                save_buses();
                exit(0);
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}

// void main_menu() {
//     int choice;
//     while (1) {
//         clearScreen();
//         printf("Main Menu\n");
//         printf("1. User Login\n");
//         printf("2. User Registration\n");
//         printf("3. Reset Password\n");
//         printf("4. Admin Login\n");
//         printf("5. Exit\n");
//         printf("Enter your choice: ");
//         scanf("%d", &choice);

//         switch (choice) {
//             case 1:
//                 // clearScreen();
//                 login();
//                 break;
//             case 2:
//                 // clearScreen();
//                 register_user();
//                 break;
//             case 3:
//                 // clearScreen();
//                 reset_password();
//                 break;
//             case 4:
//                 // clearScreen();
//                 if (adminLogin()) {
//                     adminMenu();
//                    // signoutAdmin();
//                 }
//                 break;
//             case 5:
//                 exit(0);
//                 // clearScreen();
//             default:
//                 printf("Invalid choice. Try again.\n");
//         }
//     }
// }

void user_menu(int user_idx) {
    int choice;
    while (1) {
        clearScreen();
             printf("                      ----------------------------------------------------------------------------\n\n");
 sleep(1);
 printf("                     |            **************** User Menu ******************                 |\n\n");
 sleep(2);
 printf("                     ----------------------------------------------------------------------------\n\n");
 sleep(1);
 printf("                     |                      1. View Profile                                     |\n\n");
 sleep(1);
 printf("                     |                      2. Update Profile                                   |\n\n");
 sleep(1);
 printf("                     |                      3. Make Reservation                                 |\n\n");
 sleep(1);
 printf("                     |                      4.Cancel Booking                                    |\n\n");
 sleep(1);
 printf("                     |                      5.View Booking Details                              |\n\n");
 sleep(1);
 printf("                     |                      6.Sign Out                                          |\n\n");
 printf("                     ----------------------------------------------------------------------------\n\n");
 sleep(2);
 printf("                                            Your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                view_profile(user_idx);
                break;
            case 2:
                update_profile(user_idx);
                 clearScreen();
                break;
            case 3:
                clearScreen();
                make_reservation(user_idx);
                break;
            case 4:
                clearScreen();
                cancel_booking(user_idx);
                break;
            case 5:
                clearScreen();
                view_booking_details(user_idx);
                break;
            case 6:
                sign_out(user_idx);
                return;
            default:
                printf("Invalid choice. Try again.\n");
        }
        printf("Press any key to continue...");
        getchar();
    }
}

void adminMenu() {
    int choice;
    while (1) {
        clearScreen();
                     printf("                      ----------------------------------------------------------------------------\n\n");
 sleep(1);
 printf("                     |            **************** Admin Menu ******************                 |\n\n");
 sleep(2);
 printf("                     ----------------------------------------------------------------------------\n\n");
 sleep(1);
 printf("                     |                      1. Add Bus Details                                    |\n\n");
 sleep(1);
 printf("                     |                      2. Delete Bus Details                                 |\n\n");
 sleep(1);
 printf("                     |                      3. Update Bus Details                                 |\n\n");
 sleep(1);
 printf("                     |                      4. Update Seat Status                                 |\n\n");
 sleep(1);
 printf("                     |                      5. Set Bus Prices                                     |\n\n");
 sleep(1);
 printf("                     |                      6. View Booking Log                                   |\n\n");
 sleep(1);
 printf("                     |                      7.Sign Out                                            |\n\n");
 printf("                     ----------------------------------------------------------------------------\n\n");
 sleep(2);
 printf("                                            Your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                add_bus_details();
                break;
            case 2:
                // clearScreen();
                delete_bus_details();
                break;
            case 3:
                // clearScreen();
                update_bus_details();
                break;
            case 4:
                // clearScreen();
                update_seat_status();
                break;
            case 5:
                // clearScreen();
                set_bus_prices();
                break;
            case 6:
                // clearScreen();
                view_admin_log();
                break;
            case 7:
            clearScreen();
                return;
            default:
                printf("Invalid choice. Try again.\n");
        }
        printf("Press any key to continue...");
        getchar();
    }
}

// int main() {
//     load_users();
//     load_buses();
//     load_bookings();
//     main_menu();
//     return 0;
// }