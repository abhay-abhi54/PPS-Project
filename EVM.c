#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_NAME 50
#define MAX_PIN  20

/* Structure for voter details */
typedef struct {
    int id;
    char name[MAX_NAME];
    char pin[MAX_PIN];
    int hasVoted;
} Voter;

/* Structure for candidate details */
typedef struct {
    int id;
    char name[MAX_NAME];
    int votes;
} Candidate;

/* Structure for voting transactions */
typedef struct {
    int transactionID;
    unsigned int ballotID;
    int candidateID;
    time_t timestamp;
    unsigned long checksum;
} Transaction;

/* Dynamic arrays */
Voter *voters = NULL;
Candidate *candidates = NULL;
Transaction *transactions = NULL;

int voterCount = 0;
int candidateCount = 0;
int transactionCount = 0;

unsigned long lastChecksum = 0;

/* Calculate a simple checksum */
unsigned long calculateChecksum(Transaction t,
                                unsigned long previousChecksum) {
    unsigned long hash = previousChecksum;

    hash = hash * 31 + (unsigned long)t.transactionID;
    hash = hash * 31 + (unsigned long)t.ballotID;
    hash = hash * 31 + (unsigned long)t.candidateID;
    hash = hash * 31 + (unsigned long)t.timestamp;

    return hash;
}

/* Register a voter */
void registerVoter() {
    Voter *temp;
    int id;
    char name[MAX_NAME];
    char pin[MAX_PIN];

    printf("Enter voter ID: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input.\n");
        return;
    }

    for (int i = 0; i < voterCount; i++) {
        if (voters[i].id == id) {
            printf("Voter ID already exists.\n");
            return;
        }
    }

    printf("Enter voter name: ");
    scanf(" %49[^\n]", name);

    printf("Create voter PIN: ");
    scanf("%19s", pin);

    temp = realloc(voters, (voterCount + 1) * sizeof(Voter));

    if (temp == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    voters = temp;

    voters[voterCount].id = id;
    strcpy(voters[voterCount].name, name);
    strcpy(voters[voterCount].pin, pin);
    voters[voterCount].hasVoted = 0;

    voterCount++;

    printf("Voter registered successfully.\n");
}

/* Register a candidate */
void registerCandidate() {
    Candidate *temp;
    int id;
    char name[MAX_NAME];

    printf("Enter candidate ID: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input.\n");
        return;
    }

    for (int i = 0; i < candidateCount; i++) {
        if (candidates[i].id == id) {
            printf("Candidate ID already exists.\n");
            return;
        }
    }

    printf("Enter candidate name: ");
    scanf(" %49[^\n]", name);

    temp = realloc(candidates,
                   (candidateCount + 1) * sizeof(Candidate));

    if (temp == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    candidates = temp;

    candidates[candidateCount].id = id;
    strcpy(candidates[candidateCount].name, name);
    candidates[candidateCount].votes = 0;

    candidateCount++;

    printf("Candidate registered successfully.\n");
}

/* Display candidates */
void displayCandidates() {
    printf("\n--- Candidate List ---\n");

    for (int i = 0; i < candidateCount; i++) {
        printf("ID: %d | Name: %s\n",
               candidates[i].id,
               candidates[i].name);
    }
}

/* Cast a vote */
void castVote() {
    int voterID, candidateID;
    char pin[MAX_PIN];
    int voterIndex = -1;
    int candidateIndex = -1;

    Voter *voterTemp;
    Transaction *transactionTemp;

    if (voterCount == 0 || candidateCount == 0) {
        printf("Register voters and candidates first.\n");
        return;
    }

    printf("Enter voter ID: ");
    if (scanf("%d", &voterID) != 1) {
        printf("Invalid input.\n");
        return;
    }

    for (int i = 0; i < voterCount; i++) {
        if (voters[i].id == voterID) {
            voterIndex = i;
            break;
        }
    }

    if (voterIndex == -1) {
        printf("Voter not found.\n");
        return;
    }

    if (voters[voterIndex].hasVoted) {
        printf("You have already voted.\n");
        return;
    }

    printf("Enter PIN: ");
    scanf("%19s", pin);

    if (strcmp(voters[voterIndex].pin, pin) != 0) {
        printf("Incorrect PIN.\n");
        return;
    }

    displayCandidates();

    printf("Enter candidate ID to vote for: ");
    if (scanf("%d", &candidateID) != 1) {
        printf("Invalid input.\n");
        return;
    }

    for (int i = 0; i < candidateCount; i++) {
        if (candidates[i].id == candidateID) {
            candidateIndex = i;
            break;
        }
    }

    if (candidateIndex == -1) {
        printf("Invalid candidate ID.\n");
        return;
    }

    transactionTemp = realloc(
        transactions,
        (transactionCount + 1) * sizeof(Transaction)
    );

    if (transactionTemp == NULL) {
        printf("Unable to allocate transaction memory.\n");
        return;
    }

    transactions = transactionTemp;

    Transaction t;

    t.transactionID = transactionCount + 1;
    t.ballotID = (unsigned int)rand();
    t.candidateID = candidateID;
    t.timestamp = time(NULL);

    t.checksum = calculateChecksum(t, lastChecksum);

    transactions[transactionCount] = t;
    transactionCount++;

    lastChecksum = t.checksum;

    candidates[candidateIndex].votes++;
    voters[voterIndex].hasVoted = 1;

    printf("Vote cast successfully!\n");
    printf("Your ballot reference: %u\n", t.ballotID);
}

/* Display election results */
void displayResults() {
    int totalVotes = transactionCount;

    printf("\n========== ELECTION RESULTS ==========\n");

    if (totalVotes == 0) {
        printf("No votes have been cast.\n");
    } else {
        for (int i = 0; i < candidateCount; i++) {
            printf("%s: %d votes\n",
                   candidates[i].name,
                   candidates[i].votes);
        }
    }

    printf("\nRegistered voters: %d\n", voterCount);
    printf("Total votes cast: %d\n", totalVotes);

    if (voterCount > 0) {
        printf("Turnout: %.2f%%\n",
               (double)totalVotes / voterCount * 100.0);
    } else {
        printf("Turnout: 0.00%%\n");
    }

    printf("======================================\n");
}

/* Verify the transaction audit trail */
void auditTrail() {
    unsigned long previousChecksum = 0;
    int valid = 1;

    printf("\n========== AUDIT REPORT ==========\n");

    for (int i = 0; i < transactionCount; i++) {
        Transaction t = transactions[i];

        unsigned long expectedChecksum =
            calculateChecksum(t, previousChecksum);

        if (t.transactionID != i + 1 ||
            t.checksum != expectedChecksum) {
            printf("Transaction %d: FAILED\n", i + 1);
            valid = 0;
            break;
        }

        previousChecksum = t.checksum;

        printf("Transaction %d: VERIFIED\n", i + 1);
    }

    if (valid && previousChecksum == lastChecksum) {
        printf("\nAudit Result: PASSED\n");
        printf("Transaction log checksums are valid.\n");
    } else {
        printf("\nAudit Result: FAILED\n");
        printf("Possible log modification detected.\n");
    }

    printf("==================================\n");
}

/* Display transaction log */
void displayTransactions() {
    printf("\n========== TRANSACTION LOG ==========\n");

    for (int i = 0; i < transactionCount; i++) {
        char *timestamp = ctime(&transactions[i].timestamp);

        printf("Transaction ID: %d\n",
               transactions[i].transactionID);

        printf("Ballot ID: %u\n",
               transactions[i].ballotID);

        printf("Candidate ID: %d\n",
               transactions[i].candidateID);

        printf("Timestamp: %s", timestamp);

        printf("Checksum: %lu\n\n",
               transactions[i].checksum);
    }

    if (transactionCount == 0) {
        printf("No transactions available.\n");
    }

    printf("=====================================\n");
}

/* Main menu */
int main() {
    int choice;

    srand((unsigned int)time(NULL));

    do {
        printf("\n====================================\n");
        printf(" ELECTRONIC VOTING SYSTEM\n");
        printf(" WITH AUDIT TRAIL\n");
        printf("====================================\n");
        printf("1. Register Voter\n");
        printf("2. Register Candidate\n");
        printf("3. Display Candidates\n");
        printf("4. Cast Vote\n");
        printf("5. Display Results\n");
        printf("6. Verify Audit Trail\n");
        printf("7. Display Transaction Log\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        switch (choice) {
            case 1:
                registerVoter();
                break;

            case 2:
                registerCandidate();
                break;

            case 3:
                displayCandidates();
                break;

            case 4:
                castVote();
                break;

            case 5:
                displayResults();
                break;

            case 6:
                auditTrail();
                break;

            case 7:
                displayTransactions();
                break;

            case 8:
                printf("Exiting voting system...\n");
                break;

            default:
                printf("Invalid choice. Try again.\n");
        }

    } while (choice != 8);

    free(voters);
    free(candidates);
    free(transactions);

    return 0;
}