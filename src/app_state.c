#include "app_state.h"

#define DROPDOWN_OPTIONS 64

File fileArray[10] = { 
    {.name = CLAY_STRING("Q4_Financial_Report.xlsx"), .created_at = CLAY_STRING("2023-10-15 09:30:22"), .modified_at = CLAY_STRING("2023-12-05 16:45:10"), .size = CLAY_STRING("8.7 MB") },
    {.name = CLAY_STRING("vacation_photos.zip"), .created_at = CLAY_STRING("2022-07-22 14:20:05"), .modified_at = CLAY_STRING("2022-07-22 14:20:05"), .size = CLAY_STRING("245.3 MB") },
    {.name = CLAY_STRING("project_proposal.pdf"), .created_at = CLAY_STRING("2024-01-08 11:05:17"), .modified_at = CLAY_STRING("2024-01-10 09:15:33"), .size = CLAY_STRING("3.2 MB") },
    {.name = CLAY_STRING("system_backup.iso"), .created_at = CLAY_STRING("2024-02-28 03:00:00"), .modified_at = CLAY_STRING("2024-02-28 03:45:12"), .size = CLAY_STRING("4876.5 MB") },
    {.name = CLAY_STRING("meeting_notes.txt"), .created_at = CLAY_STRING("2024-03-12 15:40:21"), .modified_at = CLAY_STRING("2024-03-12 16:20:08"), .size = CLAY_STRING("0.02 MB") },
    {.name = CLAY_STRING("database_dump.sql"), .created_at = CLAY_STRING("2024-01-25 23:15:00"), .modified_at = CLAY_STRING("2024-02-15 23:15:00"), .size = CLAY_STRING("124.8 MB") },
    {.name = CLAY_STRING("presentation_final.pptx"), .created_at = CLAY_STRING("2023-11-30 10:12:45"), .modified_at = CLAY_STRING("2023-12-02 17:30:19"), .size = CLAY_STRING("15.6 MB") },
    {.name = CLAY_STRING("software_installer.exe"), .created_at = CLAY_STRING("2024-02-10 08:00:00"), .modified_at = CLAY_STRING("2024-02-10 08:00:00"), .size = CLAY_STRING("89.3 MB") },
    {.name = CLAY_STRING("research_data.csv"), .created_at = CLAY_STRING("2023-09-05 13:25:40"), .modified_at = CLAY_STRING("2024-01-18 11:10:25"), .size = CLAY_STRING("12.4 MB") },
    {.name = CLAY_STRING("user_manual.docx"), .created_at = CLAY_STRING("2023-12-12 16:40:11"), .modified_at = CLAY_STRING("2024-02-28 10:05:29"), .size = CLAY_STRING("2.1 MB") },
};

Clay_String drives[3] = {
    CLAY_STRING("Drive C:"),
    CLAY_STRING("Drive D:"),
    CLAY_STRING("Drive F:")
};

void AppState_Initialize(AppState* app_state) {
    app_state->availableDrives = drives;
    app_state->currentFiles = fileArray;
    app_state->driveCount = 3;
    app_state->fileCount = 10;

    app_state->selectedDrive = 0;
    app_state->dropdownOpen = false;

    DropdownOptionPool options = {
        .options = malloc(sizeof(DropdownOptionData)*DROPDOWN_OPTIONS),
        .count = 0
    };
    app_state->dropdownOptions = options;
}

void AppState_Destroy(AppState* app_state) {
    free(app_state->dropdownOptions.options);
}