/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExecuteWithUnwind.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 14:42:31 by mhuszar           #+#    #+#             */
/*   Updated: 2025/02/23 20:18:13 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UNWIND_EXEC_HPP
#define UNWIND_EXEC_HPP

class ExecuteWithUnwind
{

public:
    ExecuteWithUnwind(const char *path, char **args, char **env);
    ~ExecuteWithUnwind() throw();
    ExecuteWithUnwind(const ExecuteWithUnwind& other);
    int execute();

private:
    ExecuteWithUnwind(void);
    ExecuteWithUnwind& operator=(const ExecuteWithUnwind& other);

    const char *_path;
    char **_args;
    char **_env;
};

#endif
