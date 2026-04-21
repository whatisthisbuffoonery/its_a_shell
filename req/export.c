/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achew <achew@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 01:12:45 by achew             #+#    #+#             */
/*   Updated: 2026/04/22 01:12:53 by achew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Executes the export command logic.
 * Handles strings in the format: "VAR=VALUE"
 */
void execute_export(char *arg) {
    if (arg == NULL) {
        // If no arguments, standard shell behavior is to list environment variables.
        // For simplicity, we just return here.
        printf("Usage: export NAME=VALUE\n");
        return;
    }

    // Split the input into name and value
    char *name = strtok(arg, "=");
    char *value = strtok(NULL, ""); // Get the remainder of the string

    if (name != NULL) {
        if (value == NULL) {
            // Case: export VAR (Empty string value)
            setenv(name, "", 1);
        } else {
            // Case: export VAR=VALUE
            setenv(name, value, 1);
        }
        printf("Exported: %s=%s\n", name, value ? value : "");
    } else {
        fprintf(stderr, "export: invalid identifier\n");
    }
}

int main(int argc, char *argv[]) {
    // Simulating a shell receiving the argument string
    if (argc > 1) {
        execute_export(argv[1]);
    }
    
    // Demonstrate that the variable is now in the environment
    char *val = getenv("TEST_VAR");
    if (val) {
        printf("Current environment TEST_VAR: %s\n", val);
    }
    
    return 0;
}
