//
// Created by alvis on 2021/8/7.
//

#include "repl_history.h"
#include <stdlib.h>
#include <string.h>

struct hist_list {

    char *cmd_line;

    struct hist_list* priv;
    struct hist_list* next;
};

/**
 * error number of repl history, it can only be changed internally.
 */
static int g_hist_errno = 0;

/**
 * get error number if operate history error.
 * @return errno of history operation.
 */
int hist_errno_get()
{
    return g_hist_errno;
}

/**
 * create a history buffer to store history.
 * @return 0 if success; otherwise NULL, errno will be set.
 */
hist_list_t hist_list_create()
{
    hist_list_t list = (hist_list_t) malloc(sizeof(struct hist_list));
    if (!list)
    {
        g_hist_errno = HIST_ERR_NO_MEMORY;
        return NULL;
    }

    list->next = list;
    list->priv = list;
    list->cmd_line = NULL;

    return list;
}

/**
 * destroy a history list, recovery the memory.
 * @param head list head;
 * @return 0 for success, HIST_ERRNO for failed.
 */
int hist_list_destroy(hist_list_t head)
{
    int ret = hist_node_clear(head);
    if (ret) return ret;

    free(head);

    return 0;
}

/**
 * create a history node, store the cmd_line in it;
 * @param cmd_line cmd string need to be store;
 * @param is_copy if 0, the `cmd_line` will be save in node as a pointer; otherwise will copy it.
 * @return node prt if success, otherwise NULL; errno will be set;
 */
hist_list_t hist_node_create(char *cmd_line, int is_copy)
{
    if (!cmd_line)
    {
        g_hist_errno = HIST_ERR_INVALID_NODE;
        return NULL;
    }

    hist_list_t node = (hist_list_t) malloc(sizeof(struct hist_list));
    if (!node)
    {
        g_hist_errno = HIST_ERR_NO_MEMORY;
        return NULL;
    }

    if (is_copy)
    {
        char *cmd_line_cp = (char*) malloc(strlen(cmd_line) + 1);
        if (!cmd_line_cp)
        {
            free(node);
            g_hist_errno = HIST_ERR_NO_MEMORY;
            return NULL;
        }

        strcpy(cmd_line_cp, cmd_line);
        node->cmd_line = cmd_line_cp;

    }
    else
    {
        node->cmd_line = cmd_line;
    }

    node->priv = node->next = node;
    return node;
}

/**
 * add a history node, head and node need be valid, otherwise return errno.
 * @param head history list head.
 * @param node history list node.
 * @return 0 if success, otherwise HIST_ERRNO. `errno` will not be changed by this method.
 */
int hist_node_add(hist_list_t head, hist_list_t node)
{
    if (!head || head->cmd_line != NULL) return HIST_ERR_INVALID_HEAD;
    if (!node || node->cmd_line == NULL) return HIST_ERR_INVALID_NODE;

    node->next = head;
    node->priv = head->priv;
    head->priv->next = node;
    head->priv = node;

    return 0;
}

/**
 * add a cmd line into a history list.
 * @param head history list;
 * @param cmd_line command line need to be add.
 * @param is_copy if the cmd_line need to be copy
 * @return 0 for success, HIST_ERRNO for failed.
 */
int hist_cmd_add(hist_list_t head, char *cmd_line, int is_copy)
{
    if (!head || head->cmd_line != NULL) return HIST_ERR_INVALID_HEAD;
    if (!cmd_line) return HIST_ERR_INVALID_NODE;

    hist_list_t node = (hist_list_t) malloc(sizeof(struct hist_list));
    if (!node) return HIST_ERR_NO_MEMORY;

    if (is_copy)
    {
        char *cmd_line_cp = (char*) malloc(strlen(cmd_line) + 1);
        if (!cmd_line_cp)
        {
            free(node);
            return HIST_ERR_NO_MEMORY;
        }
        strcpy(cmd_line_cp, cmd_line);
        node->cmd_line = cmd_line_cp;
    }
    else
    {
        node->cmd_line = cmd_line;
    }

    node->next = head;
    node->priv = head->priv;
    head->priv->next = node;
    head->priv = node;

    return 0;
}

/**
 * remove a node from the list contain it;
 * @param node node need to be remove;
 * @return 0 if success, otherwise HITS_ERRNO; `errno` will not be changed by this method.
 */
int hist_node_del(hist_list_t node)
{
    if (!node || node->cmd_line == NULL) return HIST_ERR_INVALID_NODE;

    node->priv->next = node->next;
    node->next->priv = node->priv;
    free(node->cmd_line);
    free(node);

    return 0;
}

/**
 * clear the history in a history buffer. this method will not destroy the list head.
 * @param head list head need to be clear;
 * @return 0 if success, otherwise HIST_ERRNO.
 */
int hist_node_clear(hist_list_t head)
{
    if (!head || head->cmd_line != NULL) return HIST_ERR_INVALID_HEAD;

    while(head->next != head)
    {
        hist_node_del(head->next);
    }

    return 0;
}



