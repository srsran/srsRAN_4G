// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_EMERGENCY_HANDLERS_H
#define SRSRAN_EMERGENCY_HANDLERS_H

using emergency_cleanup_callback = void (*)(void*);

// Add a cleanup function to be called when a kill signal is about to be delivered to the process. The handler may
// optionally pass a pointer to identify what instance of the handler is being called.
// Returns the id of the handler as a positive value, otherwise returns -1 on error.
int add_emergency_cleanup_handler(emergency_cleanup_callback callback, void* data);

// Removes the emergency handler with the specified id.
void remove_emergency_cleanup_handler(int id);

// Executes all registered emergency cleanup handlers.
void execute_emergency_cleanup_handlers();

#endif // SRSRAN_EMERGENCY_HANDLERS_H
