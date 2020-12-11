int max_line_size()
{
    FILE* TasksPriorities = NULL;
    errno_t input_error;
    input_error = fopen_s(&TasksPriorities, "TasksPriorities.txt", "r");
    char ch;
    int max = 0, i = 0;
    do
        if ((ch = fgetc(TasksPriorities)) != EOF && ch != '\n') i++;
        else {
            if (i > max) max = i;
            i = 0;
        }
    while (ch != EOF);
    fclose(TasksPriorities);
    return max + 4; //for \n and \r
}
void MainThread(int LinesNumber, Queue* Q)
{
    int i;
    int max_size = max_line_size();
    FILE* TasksPriorities = NULL;
    errno_t input_error;
    input_error = fopen_s(&TasksPriorities, "TasksPriorities.txt", "r");
    if (input_error == 0) 
    {
        char* priority = (char*)malloc((max_size * sizeof(char)));
        if (NULL != priority)
        {
            for (i = 1; i < LinesNumber + 1; i++)
            {
                fgets(priority, max_size, TasksPriorities);
                Push(Q, atoi(priority));
            }
        }
        else
            printf("Memory Allocation failure!");
    }
    else
    {
        printf("Couldn't open TasksPriorities.txt");
    }
    fclose(TasksPriorities);
}
